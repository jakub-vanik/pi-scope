#include "timer.h"

Timer::Timer() :
  running(true),
  interval(std::chrono::minutes(1)),
  thread([this]() { this->entryPoint(); })
{
}

Timer::~Timer()
{
  running.store(false);
  condition_variable.notify_one();
  thread.join();
}

void Timer::start(std::chrono::seconds interval, std::function<void()> function)
{
  std::unique_lock<std::mutex> lock(mutex);
  this->interval = interval;
  this->function = function;
  condition_variable.notify_one();
}

void Timer::stop()
{
  std::unique_lock<std::mutex> lock(mutex);
  interval = std::chrono::minutes(1);
  function = std::function<void()>();
}

void Timer::entryPoint()
{
  std::unique_lock<std::mutex> lock(mutex);
  auto next_wake = std::chrono::steady_clock::now() + interval;
  while (running.load()) {
    auto sleep_duration = next_wake - std::chrono::steady_clock::now();
    if (sleep_duration.count() > 0) {
      if (condition_variable.wait_for(lock, sleep_duration) ==
          std::cv_status::no_timeout) {
        next_wake = std::chrono::steady_clock::now();
      }
    } else {
      if (function) {
        function();
      }
      next_wake += interval;
    }
  }
}
