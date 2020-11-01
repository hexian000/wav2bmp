#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <deque>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <thread>
#include <vector>

class thread_pool {
	std::vector<std::thread> threads;
	std::deque<std::function<void()>> task_queue;

	mutable std::mutex mu;
	mutable std::condition_variable task_cond;
	mutable std::condition_variable idle_cond;

	bool done;
	unsigned int running_count;

	void thread_main();

public:
	explicit thread_pool(unsigned int n);

	void push(std::function<void()> &&task);

	void join();

	void shutdown();

	~thread_pool();
};

#endif //THREAD_POOL_HPP
