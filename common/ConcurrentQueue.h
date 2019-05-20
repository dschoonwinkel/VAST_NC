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
#include <iostream>

template <typename T>
class ConcurrentQueue
{
 public:

//  T pop()
//  {
//    if (!active)
//    {
//        std::cout << "Returning an empty item" << std::endl;
//        return T();
//    }

//    std::lock_guard<std::mutex> mlock(mutex_);
//    auto val = queue_.front();
//    queue_.pop();
//    std::cout << "Returning an item" << std::endl;
//    return val;
//  }

  //Return true if item was popped successfully
  bool pop(T& item)
  {
    if (!active)
        return false;


    if (queue_.size() <= 0)
        return false;

    std::lock_guard<std::mutex> mlock(mutex_);
    item = queue_.front();
    queue_.pop();
    return true;
  }

  void push(const T& item)
  {
    if (!active)
        return;

    std::lock_guard<std::mutex> mlock(mutex_);
    queue_.push(item);
  }

  ConcurrentQueue()=default;
  ConcurrentQueue(const ConcurrentQueue&) = delete;            // disable copying
  ConcurrentQueue& operator=(const ConcurrentQueue&) = delete; // disable assignment

  size_t size()
  {
//      std::lock_guard<std::mutex> mlock(mutex_);
      return queue_.size ();
  }

  void close()
  {
    active = false;
  }
  
 private:
  std::queue<T> queue_;
  std::mutex mutex_;
  bool active = true;
};

#endif
