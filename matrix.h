#pragma once

#include <cstddef>
#include <vector>

template <class T>
class Matrix : public std::vector<T>
{
public:
  Matrix()
  {
    size_x = 0;
    size_y = 0;
  }
  Matrix(size_t size_x, size_t size_y) : std::vector<T>(size_x * size_y)
  {
    this->size_x = size_x;
    this->size_y = size_y;
  }
  std::pair<size_t, size_t> size() const
  {
    return {size_x, size_y};
  }
  void resize(size_t size_x, size_t size_y)
  {
    this->size_x = size_x;
    this->size_y = size_y;
    std::vector<T>::resize(size_x * size_y);
  }
  T &at(size_t x, size_t y)
  {
    return std::vector<T>::at(y * size_x + x);
  }
  const T &at(size_t x, size_t y) const
  {
    return std::vector<T>::at(y * size_x + x);
  }

private:
  size_t size_x;
  size_t size_y;
};
