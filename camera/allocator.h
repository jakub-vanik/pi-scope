#pragma once

#include <condition_variable>
#include <cstdbool>
#include <mutex>

namespace camera {

template <unsigned int S>
class Allocator
{
public:
  class Token
  {
  public:
    ~Token()
    {
      allocator.release(id);
    }
    unsigned int getId()
    {
      return id;
    }

  private:
    Allocator<S> &allocator;
    unsigned int id;
    Token(Allocator<S> &allocator, unsigned int id) :
      allocator(allocator), id(id)
    {
    }
    friend class Allocator<S>;
  };
  Allocator() : used{} {}
  Token allocate()
  {
    std::unique_lock<std::mutex> lock(mutex);
    while (true) {
      for (unsigned int i = 0; i < S; i++) {
        if (!used[i]) {
          used[i] = true;
          return Token(*this, i);
        }
      }
      condition_variable.wait(lock);
    }
  }

private:
  std::mutex mutex;
  std::condition_variable condition_variable;
  bool used[S];
  void release(unsigned int id)
  {
    {
      std::unique_lock<std::mutex> lock(mutex);
      used[id] = false;
    }
    condition_variable.notify_one();
  }
};

} // namespace camera
