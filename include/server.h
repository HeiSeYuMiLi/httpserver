#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <map>
#include <string>

using boost::asio::ip::tcp;

namespace httpserver {

	class Connection;

	class Server {
	public:
		Server(const std::string& address, short port);

		//开始循环
		void Run();
		void ClearConnection(std::string);
	private:
		void DoAccept();
		void HandleAccept(std::shared_ptr<Connection>, const boost::system::error_code& ec);

		boost::asio::io_context _ioc;
		tcp::acceptor _acceptor;
		std::map<std::string, std::shared_ptr<Connection>> _connections;
	};

}