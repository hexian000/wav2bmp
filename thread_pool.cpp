#include <iostream>
#include "thread_pool.hpp"

void thread_pool::thread_main() {
	while (true) {
		std::function<void()> task;
		{
			std::unique_lock l(mu);
			running_count--;
			if (running_count == 0u) {
				idle_cond.notify_all();
			}
			if (done) {
				return;
			}
			while (task_queue.empty()) {
				task_cond.wait(l);
				if (done) {
					return;
				}
			}
			task = std::move(task_queue.front());
			task_queue.pop_front();
			running_count++;
		}
		task();
	}
}

thread_pool::thread_pool(unsigned int n) : done(false), running_count(n) {
	threads.reserve(n);
	for (decltype(n) i = 0u; i < n; ++i) {
		threads.emplace_back([this]() { thread_main(); });
	}
}

void thread_pool::push(std::function<void()> &&task) {
	{
		std::lock_guard l(mu);
		task_queue.push_back(task);
	}
	task_cond.notify_one();
}

void thread_pool::join() {
	std::unique_lock l(mu);
	while (running_count > 0u) {
		idle_cond.wait(l);
	}
}

void thread_pool::shutdown() {
	{
		std::lock_guard l(mu);
		done = true;
	}
	task_cond.notify_all();
	for (auto &t:threads) {
		t.join();
	}
}

thread_pool::~thread_pool() = default;
