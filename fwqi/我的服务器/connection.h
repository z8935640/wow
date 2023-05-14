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
	//��д��socket
	SOCKET& GetSocket();

	char* GetIP() const;	//��ȡ�ͻ���IP

	WORD GetPort() const; // ��ȡ�ͻ��˶˿ں�

	//��ȡ�Ļ�����
	void* GetReadBuffer();
	//��ȡ�������С
	std::size_t GetReadBufferSize();
	//д��Ļ�����
	void* GetWriteBuffer();
	//��ȡд�����С
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