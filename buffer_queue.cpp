#include "buffer_queue.h"

#include <cstdbool>
#include <exception>

BufferQueue::BufferQueue(unsigned int capacity) :
  push_semaphore(capacity, capacity + 1),
  pull_semaphore(0, capacity + 1),
  terminated(false)
{
}

void BufferQueue::push(Poco::Buffer<char> &&buffer)
{
  push_semaphore.wait();
  if (terminated.load()) {
    push_semaphore.set();
    throw std::exception();
  }
  std::unique_lock<std::mutex> lock(mutex);
  queue.push(buffer);
  lock.unlock();
  pull_semaphore.set();
}

Poco::Buffer<char> BufferQueue::pull()
{
  pull_semaphore.wait();
  if (terminated.load()) {
    pull_semaphore.set();
    throw std::exception();
  }
  std::unique_lock<std::mutex> lock(mutex);
  auto buffer = std::move(queue.front());
  queue.pop();
  lock.unlock();
  push_semaphore.set();
  return buffer;
}

void BufferQueue::terminate()
{
  bool expected = false;
  if (terminated.compare_exchange_strong(expected, true)) {
    push_semaphore.set();
    pull_semaphore.set();
  }
}
