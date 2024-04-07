#pragma once

#include <atomic>
#include <condition_variable>
#include <cstdbool>
#include <functional>
#include <mutex>
#include <thread>

class Timer
{
public:
  Timer();
  virtual ~Timer();
  void start(std::chrono::seconds interval, std::function<void()> function);
  void stop();

private:
  std::mutex mutex;
  std::condition_variable condition_variable;
  std::atomic<bool> running;
  std::chrono::seconds interval;
  std::function<void()> function;
  std::thread thread;
  void entryPoint();
};
