#include "app.h"
#include <ws2tcpip.h>
void App::ProcessMessage(MesageHeader* mh, Connection* conn)
{
	// ���ص��ṹ������ȡ���ͻ��� socket ����Ϣ
	//sockaddr_in* client_addr = nullptr;
	//sockaddr_in* local_addr = nullptr;
	//int client_addr_len = sizeof(sockaddr_in), local_addr_len = sizeof(sockaddr_in);
	//_iocpServer._getacceptexsockaddrs_func(conn->GetReadBuffer(), 0, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16,
	//	reinterpret_cast<LPSOCKADDR*>(&local_addr), &local_addr_len,
	//	reinterpret_cast<LPSOCKADDR*>(&client_addr), &client_addr_len);

	//// ��ȡ�ͻ��˵� IP �Ͷ˿�
	//char client_ip[16] = { 0 };
	//inet_ntop(AF_INET, &client_addr->sin_addr, client_ip, sizeof(client_ip));
	//int client_port = ntohs(client_addr->sin_port);


	//fprintf(stderr, "socket = %d, IP = %s, �˿� = %d \n", conn->GetSocket(), client_ip, client_addr->sin_port);



	MesageHeader* sendMsg = nullptr;
	switch (mh->type)
	{
	case T_Login:
	{
		Login* lg = (Login*)mh;

		//fprintf(stderr, "�ͻ���socket <%d> IP = %s, �˿� = %d, T_Login �ʺ�(%s), ����(%s), ����:%d\n", conn->GetSocket(), conn->GetIP(), conn->GetPort(), lg->name, lg->password, lg->lenth);
		sendMsg = new Login_Result();
		_iocpServer.AsyncRead(conn);
	}break;

	case T_Logout:
	{
		Logout* lg = (Logout*)mh;
		//fprintf(stderr, "�ͻ���socket <%d> T_Login �ʺ�(%s), ����(%s), ����:%d");
		sendMsg = new Logout_Result();
	}break;

	case T_ERROR:
		fprintf(stderr, "T_ERROR\n"); 
		break;
	default:
		fprintf(stderr, "δ֪��Ϣ\n");
		break;
	}
	//if (sendMsg != nullptr)
	//	_iocpServer.AsyncWrite(conn, sendMsg, sendMsg->lenth);
}
