#pragma once

#include <Poco/ActiveMethod.h>
#include <Poco/ActiveResult.h>
#include <functional>
#include <tuple>
#include <vector>

template <class R, class... Args>
class ParallelRunner
{
public:
  ParallelRunner(std::function<R(Args...)> &function) :
    function(function),
    active_method(this, &ParallelRunner<R, Args...>::activeMethod){};
  void run(Args... args)
  {
    results.push_back(active_method(std::make_tuple(args...)));
  };
  void wait()
  {
    for (auto &result : results) {
      result.wait();
    }
  }

private:
  std::function<R(Args...)> &function;
  Poco::ActiveMethod<R, std::tuple<Args...>, ParallelRunner<R, Args...>>
      active_method;
  std::vector<Poco::ActiveResult<R>> results;
  R activeMethod(const std::tuple<Args...> &args)
  {
    return std::apply(function, args);
  }
};
