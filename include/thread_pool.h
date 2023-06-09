#pragma once
#include <mutex>
#include <queue>
#include <functional>
#include <future>
#include <thread>
#include <utility>
#include <vector>


namespace httpserver {

	template<class T>
	class SafeQueue {
	public:
		SafeQueue() {}
		SafeQueue(SafeQueue&& other) {}
		~SafeQueue() {}

		// 判断队列是否为空
		bool empty() {
			std::unique_lock<std::mutex> lock(_mutex);
			return _queue.empty();
		}
		int size() {
			std::unique_lock<std::mutex> lock(_mutex);
			return _queue.size();
		}
		// 添加元素
		void enqueue(T& t) {
			std::unique_lock<std::mutex> lock(_mutex);
			_queue.emplace(t);
		}
		// 删除元素
		bool dequeue(T& t) {
			std::unique_lock<std::mutex> lock(_mutex);
			if (_queue.empty())
				return false;
			// 取出队首元素，返回队首元素值，并进行右值引用
			t = std::move(_queue.front());
			_queue.pop();
			return true;
		}

	private:
		std::queue<T> _queue;
		std::mutex _mutex;
	};

	class ThreadPool {
	public:
		ThreadPool(const int thread_num):_threads(std::vector<std::thread>(thread_num)),_shutdown(false){}
		ThreadPool(const ThreadPool&) = delete;
		ThreadPool(ThreadPool&&) = delete;
		ThreadPool& operator=(const ThreadPool&) = delete;
		ThreadPool& operator=(ThreadPool&&) = delete;

		void init() {
			for (int i = 0; i < _threads.size(); i++) {
				// 分配工作线程
				_threads.at(i) = std::thread(ThreadWorker(this, i));
			}
		}
		void shutdown() {
			_shutdown = true;
			_lock.notify_all();
			for (int i = 0; i < _threads.size(); i++) {
				if (_threads.at(i).joinable()) {
					// 将线程加入到等待队列
					_threads.at(i).join();
				}
			}
		}

		// Submit a function to be executed asynchronously by the pool
		template <typename F, typename... Args>
		auto submit(F&& f, Args &&...args) -> std::future<decltype(f(args...))>
		{
			// Create a function with bounded parameter ready to execute
			// 连接函数和参数定义，特殊函数类型，避免左右值错误
			std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
			// Encapsulate it into a shared pointer in order to be able to copy construct
			auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);
			// Warp packaged task into void function
			std::function<void()> warpper_func = [task_ptr]() {
				(*task_ptr)();
			};
			// 队列通用安全封包函数，并压入安全队列
			_queue.enqueue(warpper_func);
			// 唤醒一个等待中的线程
			_lock.notify_one();
			// 返回先前注册的任务指针
			return task_ptr->get_future();
		}

	private:
		class ThreadWorker {
		public:
			ThreadWorker(ThreadWorker* pool, int id) :_pool(pool), _id(id){}

			void operator()() {
				std::function<void()> func;
				// 是否正在取出队列中元素
				bool dequeued;
				while (!_pool->_shutdown) {
					std::unique_lock<std::mutex> lock(_pool->_mutex);
					// 如果队列是空的，阻塞当前线程
					if (_pool->_queue.empty()) {
						// 等待条件变量通知，开启线程
						_pool->_lock.wait(lock);
					}
					// 取出任务队列中的元素
					dequeued = _pool->_queue.dequeue(func);

					// 如果成功取出，执行工作函数
					if (dequeued)
						func();
				}
			}
		private:
			int _id;
			ThreadPool* _pool;
		};

		// 线程池关闭标志
		bool _shutdown;
		// 线程池是否关闭
		SafeQueue<std::function<void()>> _queue;
		// 线程休眠锁互斥变量
		std::mutex _mutex;
		// 工作线程队列
		std::vector<std::thread> _threads;
		// 线程环境锁 让线程位于休眠或唤醒的状态
		std::condition_variable _lock;
	};

}
