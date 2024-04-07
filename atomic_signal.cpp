#include "atomic_signal.h"

AtomicSignal::AtomicSignal() : trigger(false) {}

void AtomicSignal::set()
{
  trigger.store(true);
}

bool AtomicSignal::check()
{
  bool expected = true;
  return trigger.compare_exchange_strong(expected, false);
}