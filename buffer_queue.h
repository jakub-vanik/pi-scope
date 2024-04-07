#pragma once

#include <Poco/Buffer.h>
#include <Poco/Semaphore.h>
#include <atomic>
#include <mutex>
#include <queue>

class BufferQueue
{
public:
  BufferQueue(unsigned int capacity);
  void push(Poco::Buffer<char> &&buffer);
  Poco::Buffer<char> pull();
  void terminate();

private:
  Poco::Semaphore push_semaphore;
  Poco::Semaphore pull_semaphore;
  std::atomic_bool terminated;
  std::mutex mutex;
  std::queue<Poco::Buffer<char>> queue;
};
