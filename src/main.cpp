#include "logger.h"
#include "server.h"
#include "thread_pool.h"
using namespace httpserver;

bool bstop = false;
std::condition_variable cond_quit;
std::mutex mutex_quit;

int main() {
	try {
		auto pool = ThreadPool::GetInstance();
		boost::asio::io_context ioc;
		boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
		signals.async_wait([pool, &ioc](auto, auto) {
			ioc.stop();
			pool->Stop();
			std::unique_lock<std::mutex> lock(mutex_quit);
			bstop = true;
			cond_quit.notify_one();
			});
		Server s(pool->GetIOService(), "127.0.0.1", 8080);
		{
			std::unique_lock<std::mutex> lock(mutex_quit);
			while (!bstop) {
				cond_quit.wait(lock);
			}
		}
	}
	catch (std::exception& e) {
		Logger(FILE_LOCATION, log_level::error, e.what());
	}

	return 0;
}