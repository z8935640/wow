#pragma once
#ifndef _app_h_
#define _app_h_
#include "������.h"
#include "������.h"

class �û���Ϣ
{
public:
	SOCKET �׽���;

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
	
	void OnClientConnected(const Connection* conn)			//����
	{
		_iocpServer._client_count++;
		//fprintf(stderr, "�¿ͻ�������\n");
		_iocpServer.AsyncRead(conn);	//�첽��
	}

	void OnClientDisconnected(const Connection* conn)	//���ӶϿ�
	{
		auto variable_conn = const_cast<Connection*>(conn);
		SOCKET clifd = variable_conn->GetSocket();
		//fprintf(stderr, "�ͻ���:%d �Ͽ�\n", clifd);
		_iocpServer._client_count--;
	}

	void OnRead(const Connection* conn, void* data, std::size_t size)	//��
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

	void OnWrite(const Connection* conn, std::size_t bytes_transferred)//д
	{
		//�����첽��ȡ�ͻ���
		_iocpServer.AsyncRead(conn);
	}

	void ProcessMessage(MesageHeader* mh, Connection* conn);
private:
	IocpServer _iocpServer;
	// ����ͻ���������Ϣ�����ݽṹ
	std::vector<Connection*> _clients;
};

#endif





