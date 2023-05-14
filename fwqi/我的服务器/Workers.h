#pragma once
#include <MSWSock.h>
#include <WinSock2.h>
#include <Windows.h>
#include <stdio.h>
#include <assert.h>
#include "overlapped.h"
#include "SocketExFnsHunter.h"

#include "服务器.h"




//线程池类
class Workers
{
public:
	Workers(IocpServer* server)
	{
		_iocpServer = const_cast<IocpServer*>(server);
	}

	void Start();
	void ThreadProc()
	{
		DWORD bytes_transferred;
		ULONG_PTR completion_key;
		DWORD Flags = 0;
		Overlapped* overlapped = nullptr;
		while (1)
		{
			BOOL bRet = GetQueuedCompletionStatus(_iocpServer->_completion_port, &bytes_transferred, &completion_key, reinterpret_cast<LPOVERLAPPED*>(&overlapped), INFINITE);
			if (bRet == FALSE)
			{
				//客户端直接退出,没有调用closesocket正常关闭连接
				DWORD ERR = GetLastError();
				if (ERR == WAIT_TIMEOUT || ERR == ERROR_NETNAME_DELETED)	//超时或不可再用
				{
					if (overlapped != NULL)
					{
						if (overlapped->connection != NULL)
						{
							//客户端断开,调用回调函数
							if (_iocpServer->OnDisconnected)
								_iocpServer->OnDisconnected(overlapped->connection);
							closesocket(overlapped->connection->GetSocket());
							fprintf(stderr, "socket client <%d> 断开\n", overlapped->connection->GetSocket());
							delete overlapped->connection;
							overlapped = NULL;
							continue;
						}
					}
				}
			}
			else
			{
				if (overlapped->type == Overlapped::Type::Accept_type)
				{
					//新客户端连接
					_iocpServer->Accept();
					if (_iocpServer->OnConnected)
						_iocpServer->OnConnected(overlapped->connection);
					continue;
				}
				if (bytes_transferred == 0)
				{
					//客户端正常断开
					if (overlapped->connection != NULL)
					{
						if (_iocpServer->OnDisconnected)
							_iocpServer->OnDisconnected(overlapped->connection);
						delete overlapped->connection;
						overlapped->connection = NULL;
						overlapped = nullptr;
					}
					continue;
				}
				if (overlapped->type == Overlapped::Type::Read_type)
				{
					//异步读完成
					if (_iocpServer->OnRead)
						_iocpServer->OnRead(overlapped->connection, overlapped->connection->GetReadBuffer(), bytes_transferred);
					continue;
				}
				if (overlapped->type == Overlapped::Type::Write_type)
				{
					auto conn = overlapped->connection;
					if (conn != NULL)
					{
						conn->SetSentBytes(conn->GetSentBytes() + bytes_transferred);
						//判断是否只发送了一部分
						if (conn->GetSentBytes() < conn->GetTotalBytes())
						{
							//将剩余部分再发送
							overlapped->wsa_buf.len = conn->GetTotalBytes() - conn->GetSentBytes();
							overlapped->wsa_buf.buf = reinterpret_cast<CHAR*>(conn->GetWriteBuffer()) + conn->GetSentBytes();

							auto send_result = WSASend(
								conn->GetSocket(),
								&overlapped->wsa_buf, 1,
								&bytes_transferred, 0,
								reinterpret_cast<LPWSAOVERLAPPED>(overlapped), NULL);

							if (!(send_result == NULL || send_result == SOCKET_ERROR && WSAGetLastError() == WSA_IO_PENDING))
							{
								fprintf(stderr, "发送数据失败\n");
							}
						}
						else
						{
							//发送完成, 再次出发异步读
							//AsyncRead(overlapped->connection);
							if (_iocpServer->OnWrite)
								_iocpServer->OnWrite(overlapped->connection, bytes_transferred);
						}
					}
				}
			}
		}
	}

private:
	IocpServer* _iocpServer;
};


DWORD WINAPI ServerWorkerThread(LPVOID p)
{
	assert(p);
	auto workers = (Workers*)p;
	workers->ThreadProc();
	return 0;
}

void Workers::Start()	//开始
{
	SYSTEM_INFO SystemInfo;
	GetSystemInfo(&SystemInfo);
	for (size_t i = 0; i < SystemInfo.dwNumberOfProcessors * 2; i++)
	{
		HANDLE ThreadHandle;
		DWORD ThreadID;
		if ((ThreadHandle = CreateThread(NULL, 0, ServerWorkerThread, this, 0, &ThreadID)) == NULL)
		{
			printf("CreateThread 错误 %d\n", GetLastError());
			return;
		}
		CloseHandle(ThreadHandle);
	}
	_iocpServer->Accept();
}