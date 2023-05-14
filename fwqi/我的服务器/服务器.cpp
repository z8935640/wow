#include "服务器.h"
#include <WinSock2.h>

#include "overlapped.h"
#include <memory>
#include "Workers.h"
#include "SocketExFnsHunter.h"
#pragma warning(disable:4996)

int IocpServer::Init(const char* ip, unsigned short port, unsigned int nListen)
{
	int ret = 0;
	do
	{
		ret = WinSockInit();
		if (ret == -1)
		{
			fprintf(stderr, "初始化WinSockInit失败\n");
			break;
		}
		_completion_port = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);		//创建完成端口
		if (!_completion_port)
		{
			fprintf(stderr, "创建完成端口失败\n");
			ret = -1;
			break;
		}
		if ((ret = InitSocket()) == -1)			//初始化服务器要用的Socket
			break;

		if ((ret = Bind(ip, port)) == -1)
			break;

		if ((ret = Listen(nListen)) == -1)
			break;

		SocketExFnsHunter _socketExFnsHunter;
		
		_acceptex_func = _socketExFnsHunter.AcceptEx;
		_getacceptexsockaddrs_func = _socketExFnsHunter.GetAcceptExSockaddrs;
		Workers* _workers = new Workers(this);
		_workers->Start();
	} while (0);
	return ret;
}

int IocpServer::WinSockInit()
{
	int ret = -1;
	do
	{
		WORD version = MAKEWORD(2, 2);
		WSADATA wsaData;
		_wsa_inited = !WSAStartup(version, &wsaData);
		if (!_wsa_inited)
			break;
		if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion != 2))
		{
			WSACleanup();
			_wsa_inited = false;
			break;
		}
		ret = 0;
	} while (0);
	return ret;
}

int IocpServer::InitSocket()
{
	int ret = 0;
	do
	{
		//创建服务器套接字, 这里要注意的是最后一个参数必须为: WSA_FLAG_OVERLAPPED 重叠模式
		_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (_socket == INVALID_SOCKET)
		{
			fprintf(stderr, "创建 WSASocket( listenSocket ) 失败\n");
			ret = -1;
			break;
		}
		if (!CreateIoCompletionPort(reinterpret_cast<HANDLE>(_socket), _completion_port, 0, 0 ))
		{
			fprintf(stderr, "将 listen socket关联到完成端口失败\n");
			ret = -1;
		}
	} while (0);
	return ret;
}

int IocpServer::Accept()
{
	int ret = -1;
	do
	{
		//创建一个用于链接的socket
		SOCKET accepted_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (accepted_socket == INVALID_SOCKET)
		{
			ret = -1;
			fprintf(stderr, "初始化Accept socket函数地址失败\n");
			break;
		}
		std::unique_ptr<Connection> new_connection(new Connection(accepted_socket));

		DWORD bytes = 0;
		const int accept_ex_result = _acceptex_func(
			_socket, 
			accepted_socket, 
			new_connection->GetReadBuffer(),
			0, 
			sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16,
			&bytes, reinterpret_cast<LPOVERLAPPED>(new_connection->GetAcceptOverlapped()));
		if (!(accept_ex_result == TRUE || WSAGetLastError() == WSA_IO_PENDING))
		{
			ret = -1;
			fprintf(stderr, "调用acceptex函数失败\n");
			break;
		}
		//将accept_socket关联到完成端口
		CreateIoCompletionPort(reinterpret_cast<HANDLE>(accepted_socket), _completion_port, 0, 0);
		new_connection.release();
	} while (0);
	return ret;
}

int IocpServer::Bind(const char* ip, unsigned short port)
{
	int ret = INVALID_SOCKET;
	SOCKADDR_IN addr;
	addr.sin_addr.S_un.S_addr = inet_addr(ip);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);

	if (bind(_socket, (SOCKADDR*)&addr, sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		fprintf(stderr, "Bind失败\n");
		return -1;
	}
	return 0;
}

int IocpServer::Listen(unsigned int nListen)
{
	if (SOCKET_ERROR == listen(_socket, nListen))
	{
		fprintf(stderr, "Listen失败\n");
		return -1;
	}
	return 0;
}

void IocpServer::Run(const char* ip, unsigned short port, unsigned int nListen = 5)
{
	if (Init(ip, port, nListen) == -1)
	{
		printf("服务器启动失败...\n");
		return;
	}
	Mainloop();
}

void IocpServer::Mainloop()
{
	//主循环
	for (;;)
	{
		if (_chrono_timer.GetMicroSecInterval() >= 1000000)
		{
			fprintf(stderr, "客户端数量 = %d\n", _client_count);
			_chrono_timer.FlushTime();
			_msg_count = 0;
		}
	}
}

void IocpServer::AsyncRead(const Connection* conn)
{
	auto overlapped = conn->GetReadOverlapped();
	overlapped->wsa_buf.len = overlapped->connection->GetReadBufferSize();
	overlapped->wsa_buf.buf = reinterpret_cast<CHAR*>(overlapped->connection->GetReadBuffer());

	DWORD flags = 0;
	DWORD bytes_transferred = 0;

	auto recv_result = WSARecv(overlapped->connection->GetSocket(), 
		&overlapped->wsa_buf, 1, &bytes_transferred, &flags, 
		reinterpret_cast<LPWSAOVERLAPPED>(overlapped), NULL);

	if (!(recv_result == 0 || (recv_result == SOCKET_ERROR && WSAGetLastError() == WSA_IO_PENDING)))
		fprintf(stderr, "接收数据失败<AsyncRead>失败\n");
}

void IocpServer::AsyncWrite(const Connection* conn, void* data, std::size_t size)
{
	auto mutable_conn = const_cast<Connection*>(conn);
	//分配缓冲区
	if (mutable_conn->GetWriteBufferSize() < size)
		mutable_conn->ResizeWriteBuffer(size);
	//将要发送的数据拷贝到发送缓冲区
	memcpy_s(mutable_conn->GetWriteBuffer(), mutable_conn->GetWriteBufferSize(), data, size);
	mutable_conn->SetSentBytes(0);
	mutable_conn->SetToTalBytes(size);
	//获取写重叠IO的数据结构指针
	auto overlapped = mutable_conn->GetWriteOverlapped();
	overlapped->wsa_buf.len = size;
	overlapped->wsa_buf.buf = reinterpret_cast<CHAR*>(mutable_conn->GetWriteBuffer());

	DWORD bytes;
	auto send_result = WSASend(
		mutable_conn->GetSocket(), 
		&overlapped->wsa_buf, 1, 
		&bytes, 0, 
		reinterpret_cast<LPWSAOVERLAPPED>(overlapped), NULL);

	if (!(send_result == 0 || (send_result == SOCKET_ERROR && WSAGetLastError() == WSA_IO_PENDING)))
		fprintf(stderr, "发送数据失败<AsyncWrite>失败\n");
}

