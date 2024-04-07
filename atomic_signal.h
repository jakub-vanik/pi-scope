#pragma once

#include <atomic>
#include <cstdbool>

class AtomicSignal
{
public:
  AtomicSignal();
  void set();
  bool check();

private:
  std::atomic<bool> trigger;
};
