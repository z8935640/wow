
#ifndef _IocpServer_h_
#define _IocpServer_h_
#include <WinSock2.h>
#include <windows.h>
#include <MSWSock.h>
#include <io.h>
#include <thread>
#include <iostream>
#include "connection.h"
#include "callbacks.h"
#include "ChronoTimer.h"
#pragma comment(lib, "ws2_32.lib")

class IocpServer :public CallbacksExecutor
{
public:
	IocpServer()
	{
		_wsa_inited = false;
		_socket = INVALID_SOCKET;
		_client_count = 0;	//客户端个数
		_msg_count = 0;		//消息个数    
	}
	~IocpServer() {};

public:
	int Init(const char* ip, unsigned short port, unsigned int nListen);

	int WinSockInit();

	int InitSocket();

	int Accept();

	int Bind(const char* ip, unsigned short port);

	int Listen(unsigned int nListen);

	void Run(const char* ip, unsigned short port, unsigned int nListen);

	void Mainloop();

	void AsyncRead(const Connection* conn);

	void AsyncWrite(const Connection* conn, void* data, std::size_t size);
public:
	bool _wsa_inited;
	HANDLE _completion_port = 0;
	SOCKET _socket;

	LPFN_ACCEPTEX _acceptex_func = nullptr;
	LPFN_GETACCEPTEXSOCKADDRS _getacceptexsockaddrs_func = nullptr;
	int _client_count = 0;			//客户端个数
	int _msg_count = 0;				//消息个数
	ChronoTimer _chrono_timer;	//计时器类
};
#endif

