#pragma once
#ifndef _app_h_
#define _app_h_
#include "服务器.h"
#include "操作码.h"

class 用户信息
{
public:
	SOCKET 套接字;

};
class App
{
public:
	App()
	{
		_iocpServer.SetOnConnectedCallback(std::bind(&App::OnClientConnected, this, std::placeholders::_1));
		_iocpServer.SetOnDisconnectedCallback(std::bind(&App::OnClientDisconnected, this, std::placeholders::_1));
		_iocpServer.SetOnReadCallback(std::bind(&App::OnRead, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		_iocpServer.SetOnWriteCallback(std::bind(&App::OnWrite, this, std::placeholders::_1, std::placeholders::_2));
	}
	~App() {};
public:
	void Run(const char* ip, unsigned short port, unsigned int nListen = 5)
	{
		_iocpServer.Run(ip, port, nListen);
	}
	
	void OnClientConnected(const Connection* conn)			//连接
	{
		_iocpServer._client_count++;
		//fprintf(stderr, "新客户端连接\n");
		_iocpServer.AsyncRead(conn);	//异步读
	}

	void OnClientDisconnected(const Connection* conn)	//连接断开
	{
		auto variable_conn = const_cast<Connection*>(conn);
		SOCKET clifd = variable_conn->GetSocket();
		//fprintf(stderr, "客户端:%d 断开\n", clifd);
		_iocpServer._client_count--;
	}

	void OnRead(const Connection* conn, void* data, std::size_t size)	//读
	{
		auto variable_conn = const_cast<Connection*>(conn);
		MesageHeader* mh = nullptr;
		int msgPos = variable_conn->GetMsgPos();
		memcpy(variable_conn->GetMsgBuffer() + msgPos, data, size);
		variable_conn->SetMsgPos(msgPos + size);
		while (variable_conn->GetMsgPos() >= sizeof(MesageHeader))
		{
			mh = (MesageHeader*)variable_conn->GetMsgBuffer();
			if (mh->lenth <= variable_conn->GetMsgPos())
			{
				variable_conn->SetMsgPos(variable_conn->GetMsgPos() - mh->lenth);
				ProcessMessage(mh, variable_conn);
				memcpy(variable_conn->GetMsgBuffer(), variable_conn->GetMsgBuffer() + mh->lenth, variable_conn->GetMsgPos());
			}
			else
			{
				break;
			}
		}
	}

	void OnWrite(const Connection* conn, std::size_t bytes_transferred)//写
	{
		//继续异步读取客户端
		_iocpServer.AsyncRead(conn);
	}

	void ProcessMessage(MesageHeader* mh, Connection* conn);
private:
	IocpServer _iocpServer;
	// 保存客户端连接信息的数据结构
	std::vector<Connection*> _clients;
};

#endif





