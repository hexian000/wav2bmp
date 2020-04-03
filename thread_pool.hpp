#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <deque>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <thread>
#include <vector>

class thread_pool {
  typedef std::function<void()> task_t;

  unsigned int n;

  std::vector<std::thread> threads;

  mutable std::mutex queue_mutex;

  mutable std::condition_variable task_cond;

  std::deque<task_t> task_queue;

  bool done;

  void thread_main();

 public:
  explicit thread_pool(unsigned int n);

  void push(task_t &&task);

  void shutdown();

  ~thread_pool();
};

#endif //THREAD_POOL_HPP
