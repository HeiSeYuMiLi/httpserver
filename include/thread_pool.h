#pragma once
#include <memory>
#include <thread>
#include <vector>
#include <boost/asio.hpp>
#include "singleton.h"

namespace httpserver {

	class ThreadPool:public Singleton<ThreadPool> {
	public:
		// std::thread::hardware_concurrency()表示电脑核数
		ThreadPool(int threadNum = std::thread::hardware_concurrency());
		//ThreadPool(const ThreadPool&) = delete;
		//ThreadPool& operator=(const ThreadPool&) = delete;

		boost::asio::io_context& GetIOService();
		void Stop();
	private:
		boost::asio::io_context _service;
		std::unique_ptr<boost::asio::io_context::work> _work;
		std::vector<std::thread> _threads;
	};

}
