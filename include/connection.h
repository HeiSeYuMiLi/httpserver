#pragma once

#include <boost/asio.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <string>
#include <memory>
#include "request.h"
#include "reply.h"
#include "request_parser.h"
#include "request_handler.h"

using boost::asio::ip::tcp;

namespace httpserver {

	class Server;

	class Connection : public std::enable_shared_from_this<Connection> {
	public:

		Connection(boost::asio::io_context& ioc,Server* server);
		~Connection();
		
		tcp::socket& GetSocket();
		std::string& GetUuid();
		void Start();
		void Close();
		std::shared_ptr<Connection> SharedSelf();
	private:
		void HandleRead(const boost::system::error_code& ec, size_t bytes_transferred, std::shared_ptr<Connection> shared_self);
		void HandleWrite(const boost::system::error_code& ec, std::shared_ptr<Connection> shared_self);

		tcp::socket _sock;
		Server* _server;
		std::string _uuid;
		Request _req;
		Reply _rep;
		char _buff[8912];
		RequestParser _requestParser;
		RequestHandler _requestHandler;
	};

}