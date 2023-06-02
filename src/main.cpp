#include "logger.h"
#include "server.h"
using namespace httpserver;


int main() {
	try {
		Server s("127.0.0.1", 8080);
		s.Run();
	}
	catch (std::exception& e) {
		Logger(FILE_LOCATION, log_level::error, e.what());
	}

	return 0;
}