#pragma once
#ifndef  _callbacks_h_
#define _callbacks_h_
#include <functional>
#include "connection.h"
namespace Callback
{
	typedef std::function<void(const Connection*, std::size_t)> OnWrite;		//写入时
	typedef std::function<void(const Connection*, void*, std::size_t)> OnRead;	//读入时
	typedef std::function<void(const Connection*)> OnConnected;					//连接时
	typedef std::function<void(const Connection*)> OnDisconnected;				//连接断开时
}

class CallbacksExecutor
{
public:
	CallbacksExecutor()
	{
		OnWrite = nullptr;
		OnRead = nullptr;
		OnConnected = nullptr;
		OnDisconnected = nullptr;
	}
	virtual ~CallbacksExecutor() {};
public:
	void SetOnWriteCallback(Callback::OnWrite callback)
	{
		OnWrite = callback;
	}
	void SetOnReadCallback(Callback::OnRead callback)
	{
		OnRead = callback;
	}
	void SetOnConnectedCallback(Callback::OnConnected callback)
	{
		OnConnected = callback;
	}
	void SetOnDisconnectedCallback(Callback::OnDisconnected callback)
	{
		OnDisconnected = callback;
	}
public:
	Callback::OnWrite OnWrite;
	Callback::OnRead OnRead;
	Callback::OnConnected OnConnected;
	Callback::OnDisconnected OnDisconnected;
};
#endif // ! _callbacks_h_
