#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>

using namespace std;

// 线程池
class ThreadPool {
private:
  atomic<bool> stop;             // 是否停止
  vector<thread> workers;        // 线程
  queue<function<void()>> tasks; // 任务队列

  mutex queue_mutex;            // 任务队列锁
  condition_variable condition; // 条件变量

public:
  ThreadPool(size_t num_threads);

  ~ThreadPool();

  template <class F, class... Args>
  auto enqueue(F &&f, Args &&...args)
      -> future<typename result_of<F(Args...)>::type> {
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<return_type> res = task->get_future();
    {
      std::unique_lock<std::mutex> lock(queue_mutex);

      if (stop)
        throw std::runtime_error("enqueue on stopped ThreadPool");

      tasks.emplace([task]() { (*task)(); });
    }
    condition.notify_one();
    return res;
  }
};