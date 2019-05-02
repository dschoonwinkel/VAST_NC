//
// Copyright (c) 2013 Juan Palacios juan.palacios.puyana@gmail.com
// Subject to the BSD 2-Clause License
// - see < http://opensource.org/licenses/BSD-2-Clause>
//

#ifndef CONCURRENT_QUEUE_
#define CONCURRENT_QUEUE_

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

template <typename T>
class ConcurrentQueue
{
 public:

  T pop() 
  {
    std::unique_lock<std::mutex> mlock(mutex_);
    while (queue_.empty() && active) {}
    if (!active)
        return T();

    auto val = queue_.front();
    queue_.pop();
    return val;
  }

  void pop(T& item)
  {
    std::unique_lock<std::mutex> mlock(mutex_);
    while (queue_.empty() && active) {}

    if (!active)
        return;

    item = queue_.front();
    queue_.pop();
  }

  void push(const T& item)
  {
    std::unique_lock<std::mutex> mlock(mutex_);
    queue_.push(item);
    mlock.unlock();
    cond_.notify_one();
  }
  ConcurrentQueue()=default;
  ConcurrentQueue(const ConcurrentQueue&) = delete;            // disable copying
  ConcurrentQueue& operator=(const ConcurrentQueue&) = delete; // disable assignment

  size_t size()
  {
      std::unique_lock<std::mutex> mlock(mutex_);
      return queue_.size ();
  }

  void close()
  {
    active = false;
    cond_.notify_all ();
  }
  
 private:
  std::queue<T> queue_;
  std::mutex mutex_;
  std::condition_variable cond_;
  bool active = true;
};

#endif
