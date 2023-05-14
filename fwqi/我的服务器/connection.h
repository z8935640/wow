#pragma once
#ifndef _connection_h_
#define _connection_h_

#include <memory>
#include <WinSock2.h>
#define ReadBufferSize 1024

struct Overlapped;


class Connection
{
public:
	Connection(const SOCKET& socket);
	~Connection();
public:
	//读写的socket
	SOCKET& GetSocket();

	char* GetIP() const;	//获取客户端IP

	WORD GetPort() const; // 获取客户端端口号

	//读取的缓冲区
	void* GetReadBuffer();
	//获取读缓冲大小
	std::size_t GetReadBufferSize();
	//写入的缓冲区
	void* GetWriteBuffer();
	//获取写缓冲大小
	std::size_t GetWriteBufferSize() const;

	void ResizeWriteBuffer(std::size_t new_size);

	std::size_t GetSentBytes() const;

	void SetSentBytes(std::size_t value);

	std::size_t GetTotalBytes() const;

	void SetToTalBytes(std::size_t value);

	Overlapped* GetConnectOverLapped() const;

	Overlapped* GetAcceptOverlapped() const;

	Overlapped* GetReadOverlapped() const;

	Overlapped* GetWriteOverlapped() const;

	char* GetMsgBuffer();

	int GetMsgPos() const;

	void SetMsgPos(int pos); 

private:
	class Impl;
	std::unique_ptr<Impl> _impl;
};


#endif