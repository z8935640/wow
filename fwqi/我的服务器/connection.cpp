
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "connection.h"
#include "overlapped.h"
#include <ws2tcpip.h>

#include "SocketExFnsHunter.h"
class  Connection::Impl
{
public:
	 Impl(const SOCKET& socket, Connection* owner);
	~Impl();
public:
	SOCKET _socket;

	char _read_buffer[ReadBufferSize * 4] = {};

	char _msg_buff[ReadBufferSize * 4] = {};		//消息缓冲区

	int _pos;


	std::size_t _write_buffer_size;

	std::unique_ptr<char> _write_buffer;

	std::size_t _sent_bytes;

	std::size_t _total_bytes;

	char _ip[INET_ADDRSTRLEN];	//客户端IP

	WORD _port; // 客户端端口号

	std::unique_ptr<Overlapped> _connect_overlapped;
	std::unique_ptr<Overlapped> _accept_overlapped;
	std::unique_ptr<Overlapped> _read_overlapped;
	std::unique_ptr<Overlapped> _write_overlapped;
};

 Connection::Impl::Impl(const SOCKET& socket, Connection* owner)
	 : _socket(socket)
	 , _connect_overlapped(CreateOverlapped(Overlapped::Connect_type))
	 , _accept_overlapped(CreateOverlapped(Overlapped::Accept_type))
	 , _read_overlapped(CreateOverlapped(Overlapped::Read_type))
	 , _write_overlapped(CreateOverlapped(Overlapped::Write_type))
	 , _sent_bytes()
	 , _total_bytes()
	 , _write_buffer(nullptr)
	 , _write_buffer_size()
 {
	 _connect_overlapped->connection = owner;
	 _accept_overlapped->connection = owner;
	 _read_overlapped->connection = owner;
	 _write_overlapped->connection = owner;


 }

 Connection::Impl::~Impl()
 {
	 if (_socket)
		 closesocket(_socket);
 }

 Connection::Connection(const SOCKET& socket)
	 : _impl(new Impl(socket, this)){}

 Connection::~Connection()
 {
 }

 SOCKET& Connection::GetSocket()
 {
	 return _impl->_socket;
 }

 char* Connection::GetIP() const
 {
	 return _impl->_ip;
 }

 WORD Connection::GetPort() const
 {
	 return _impl->_port;
 }

 void* Connection::GetReadBuffer()
 {
	 return &_impl->_read_buffer;
 }

 std::size_t Connection::GetReadBufferSize()
 {
	 return ReadBufferSize;
 }

 void* Connection::GetWriteBuffer()
 {
	 return _impl->_write_buffer.get();
 }

 void Connection::ResizeWriteBuffer(std::size_t new_size)
 {
	 _impl->_write_buffer.reset(new char[new_size]);	//释放原先持有的指针, 然后指向新指针,防止内存泄漏
	 _impl->_write_buffer_size = new_size;
 }

 std::size_t Connection::GetSentBytes() const
 {
	 return _impl->_sent_bytes;
 }

 std::size_t Connection::GetWriteBufferSize() const
 {
	 return _impl->_write_buffer_size;
 }

 void Connection::SetSentBytes(std::size_t value)
 {
	 _impl->_sent_bytes = value;
 }

 std::size_t Connection::GetTotalBytes() const
 {
	 return _impl->_total_bytes;
 }

 void Connection::SetToTalBytes(std::size_t value)
 {
	 _impl->_total_bytes = value;
 }

 Overlapped* Connection::GetConnectOverLapped() const
 {
	 return _impl->_connect_overlapped.get();
 }

 Overlapped* Connection::GetAcceptOverlapped() const
 {
	 return _impl->_accept_overlapped.get();;
 }

 Overlapped* Connection::GetReadOverlapped() const
 {
	 return _impl->_read_overlapped.get();
 }

 Overlapped* Connection::GetWriteOverlapped() const
 {
	 return _impl->_write_overlapped.get();
 }

 char* Connection::GetMsgBuffer()
 {
	 return _impl->_msg_buff;
 }

 int Connection::GetMsgPos() const
 {
	 return _impl->_pos;
 }

 void Connection::SetMsgPos(int pos)
 {
	_impl->_pos = pos;
 }
