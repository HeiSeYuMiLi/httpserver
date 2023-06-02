#include "connection.h"
#include "server.h"
#include "logger.h"

namespace httpserver {

	Connection::Connection(boost::asio::io_context& ioc, Server* server) :_sock(ioc), _server(server) {
		boost::uuids::uuid  a_uuid = boost::uuids::random_generator()();
		_uuid = boost::uuids::to_string(a_uuid);
	}

	Connection::~Connection() {
	}

	tcp::socket& Connection::GetSocket() {
		return _sock;
	}

	std::string& Connection::GetUuid() {
		return _uuid;
	}

	void Connection::Close() {
		_sock.close();
	}

	std::shared_ptr<Connection> Connection::SharedSelf() {
		return shared_from_this();
	}

	void Connection::Start() {
		_sock.async_read_some(boost::asio::buffer(_buff), std::bind(&Connection::HandleRead, this,
			std::placeholders::_1, std::placeholders::_2, SharedSelf()));
	}

	void Connection::HandleRead(const boost::system::error_code& ec, size_t bytes_transferred,
		std::shared_ptr<Connection> shared_self) {
		if (!ec) {
			if (_requestParser.Parse(_req, _buff))
				_requestHandler.HandleRequest(_req, _rep);
			else
				_rep = Reply::stock_reply(Reply::status_type::bad_request);
			boost::asio::async_write(_sock, _rep.to_buffers(), std::bind(&Connection::HandleWrite,
				this, std::placeholders::_1, SharedSelf()));
		}
		else if (ec != boost::asio::error::operation_aborted)
		{
			Logger(FILE_LOCATION, log_level::info, ec.message());
			Close();
			_server->ClearConnection(_uuid);
		}
	}

	void Connection::HandleWrite(const boost::system::error_code& ec, std::shared_ptr<Connection> shared_self) {
		if (!ec)
		{
			// 启动优雅的连接关闭
			boost::system::error_code ignored_ec;
			_sock.shutdown(boost::asio::ip::tcp::socket::shutdown_both,
				ignored_ec);
		}

		if (ec != boost::asio::error::operation_aborted)
		{
			Close();
			_server->ClearConnection(_uuid);
		}
	}
}