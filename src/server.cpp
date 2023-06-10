#include "server.h"
#include "connection.h"
#include "logger.h"
#include "thread_pool.h"

namespace httpserver {

	Server::Server(const std::string& address, short port) :_ioc(1), _acceptor(_ioc, 
		tcp::endpoint(boost::asio::ip::address::from_string(address), port)) {
		Logger(FILE_LOCATION, log_level::info, "Server start success, listen on port : " + std::to_string(port));
		DoAccept();
	}

	void Server::DoAccept() {
		std::shared_ptr<Connection> new_connection = std::make_shared<Connection>(_ioc, this);
		_acceptor.async_accept(new_connection->GetSocket(), std::bind(&Server::HandleAccept,
			this, new_connection, std::placeholders::_1));
	}

	void Server::HandleAccept(std::shared_ptr<Connection> new_connection, const boost::system::error_code& ec) {
		if (!ec) {
			new_connection->Start();
			_connections.insert(make_pair(new_connection->GetUuid(), new_connection));
		}
		else {
			Logger(FILE_LOCATION, log_level::error, "Session accept failed, error is " + ec.message());
		}
		DoAccept();
	}

	void Server::Run() {
		_ioc.run();
	}

	void Server::ClearConnection(std::string uuid) {
		_connections.erase(uuid);
	}

}