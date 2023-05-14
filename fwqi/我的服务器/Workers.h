#pragma once
#include <MSWSock.h>
#include <WinSock2.h>
#include <Windows.h>
#include <stdio.h>
#include <assert.h>
#include "overlapped.h"
#include "SocketExFnsHunter.h"

#include "������.h"




//�̳߳���
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
				//�ͻ���ֱ���˳�,û�е���closesocket�����ر�����
				DWORD ERR = GetLastError();
				if (ERR == WAIT_TIMEOUT || ERR == ERROR_NETNAME_DELETED)	//��ʱ�򲻿�����
				{
					if (overlapped != NULL)
					{
						if (overlapped->connection != NULL)
						{
							//�ͻ��˶Ͽ�,���ûص�����
							if (_iocpServer->OnDisconnected)
								_iocpServer->OnDisconnected(overlapped->connection);
							closesocket(overlapped->connection->GetSocket());
							fprintf(stderr, "socket client <%d> �Ͽ�\n", overlapped->connection->GetSocket());
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
					//�¿ͻ�������
					_iocpServer->Accept();
					if (_iocpServer->OnConnected)
						_iocpServer->OnConnected(overlapped->connection);
					continue;
				}
				if (bytes_transferred == 0)
				{
					//�ͻ��������Ͽ�
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
					//�첽�����
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
						//�ж��Ƿ�ֻ������һ����
						if (conn->GetSentBytes() < conn->GetTotalBytes())
						{
							//��ʣ�ಿ���ٷ���
							overlapped->wsa_buf.len = conn->GetTotalBytes() - conn->GetSentBytes();
							overlapped->wsa_buf.buf = reinterpret_cast<CHAR*>(conn->GetWriteBuffer()) + conn->GetSentBytes();

							auto send_result = WSASend(
								conn->GetSocket(),
								&overlapped->wsa_buf, 1,
								&bytes_transferred, 0,
								reinterpret_cast<LPWSAOVERLAPPED>(overlapped), NULL);

							if (!(send_result == NULL || send_result == SOCKET_ERROR && WSAGetLastError() == WSA_IO_PENDING))
							{
								fprintf(stderr, "��������ʧ��\n");
							}
						}
						else
						{
							//�������, �ٴγ����첽��
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

void Workers::Start()	//��ʼ
{
	SYSTEM_INFO SystemInfo;
	GetSystemInfo(&SystemInfo);
	for (size_t i = 0; i < SystemInfo.dwNumberOfProcessors * 2; i++)
	{
		HANDLE ThreadHandle;
		DWORD ThreadID;
		if ((ThreadHandle = CreateThread(NULL, 0, ServerWorkerThread, this, 0, &ThreadID)) == NULL)
		{
			printf("CreateThread ���� %d\n", GetLastError());
			return;
		}
		CloseHandle(ThreadHandle);
	}
	_iocpServer->Accept();
}