#include "thread_pool.hpp"

void thread_pool::thread_main() {
  while (true) {
    std::unique_lock l(queue_mutex);
    while (task_queue.empty()) {
      if (done) {
        return;
      }
      task_cond.wait(l);
    }
    auto task = task_queue.front();
    task_queue.pop_front();
    l.unlock();
    task();
  }
}

thread_pool::thread_pool(unsigned int n) : n(n), done(false) {
  threads.reserve(n);
  for (decltype(n) i = 0u; i < n; ++i) {
    threads.emplace_back([this]() { thread_main(); });
  }
}

void thread_pool::push(thread_pool::task_t &&task) {
  {
    std::lock_guard l(queue_mutex);
    task_queue.push_back(task);
  }
  task_cond.notify_one();
}

void thread_pool::shutdown() {
  {
    std::lock_guard l(queue_mutex);
    done = true;
  }
  task_cond.notify_all();
  for (auto &t:threads) {
    t.join();
  }
}

thread_pool::~thread_pool() = default;
