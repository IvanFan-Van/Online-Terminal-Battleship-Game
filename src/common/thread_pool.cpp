#include "common/thread_pool.h"

ThreadPool::ThreadPool(size_t num_threads) : stop(false) {
  for (size_t i = 0; i < num_threads; i++) {
    workers.emplace_back([this] {
      while (true) {
        function<void()> task;
        {
          // 获取任务
          unique_lock<mutex> lock(this->queue_mutex);
          this->condition.wait(lock, [this] {
            // 等待任务队列不为空或者停止
            return this->stop.load() || !this->tasks.empty();
          });
          if (this->stop.load() && this->tasks.empty()) {
            return;
          }
          task = move(this->tasks.front());
          this->tasks.pop();
        }
        task();
      }
    });
  }
}

ThreadPool::~ThreadPool() {
  {
    unique_lock<mutex> lock(queue_mutex);
    stop.store(true);
  }
  condition.notify_all();
  for (thread &worker : workers) {
    worker.join();
  }
}