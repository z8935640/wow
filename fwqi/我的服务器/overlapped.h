#pragma once
#include <WinSock2.h>
#include "connection.h"
#define ReadBufferSize 1024

struct Overlapped
{
	enum Type
	{
		Connect_type,	//连接
		Accept_type,	//接收连接
		Read_type,		//读取
		Write_type,		//写入
	};
	WSAOVERLAPPED overlapped;

	Type type;

	WSABUF wsa_buf;

	SOCKET _accepted_socket;		//用于读写的socket

	Connection* connection;

};


inline Overlapped* CreateOverlapped(Overlapped::Type type)
{
	Overlapped* overlapped = new Overlapped;
	memset(overlapped, 0, sizeof(Overlapped));
	overlapped->type = type;
	return overlapped;
}