#include "thread_pool.h"

namespace httpserver {

	ThreadPool::ThreadPool(int threadNum) :_work(new boost::asio::io_context::work(_service)) {
		for (int i = 0; i < threadNum; i++) {
			_threads.emplace_back([this]() {
				_service.run();
				});
		}
	}

	boost::asio::io_context& ThreadPool::GetIOService() {
		return _service;
	}

	void ThreadPool::Stop() {
		_work.reset();
		for (auto& t : _threads) {
			t.join();
		}
	}

}