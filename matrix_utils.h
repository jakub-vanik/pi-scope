#pragma once

#include "matrix.h"
#include "parallel_runner.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <functional>
#include <thread>

template <class T>
class MatrixUtils
{
public:
  static Matrix<T> createGausianKernel(size_t size, float variance, T scale)
  {
    Matrix<T> matrix(size, size);
    float middle = size * 0.5f;
    for (size_t y = 0; y < size; y++) {
      float dy = y - middle;
      for (size_t x = 0; x < size; x++) {
        float dx = x - middle;
        float value = std::exp(-(dx * dx / (2 * variance * variance) +
                                 dy * dy / (2 * variance * variance))) /
                      (variance * std::sqrt(2 * M_PI));
        matrix.at(x, y) = std::round(scale * value);
      }
    }
    return matrix;
  };
  static Matrix<T> createLaplacianOperator()
  {
    Matrix<T> matrix(3, 3);
    for (size_t y = 0; y < 3; y++) {
      for (size_t x = 0; x < 3; x++) {
        matrix.at(x, y) = (x == 1 && y == 1) ? 8 : -1;
      }
    }
    return matrix;
  };
  static Matrix<T> convolution(Matrix<T> a, Matrix<T> b)
  {
    auto a_size = a.size();
    auto b_size = b.size();
    Matrix<T> matrix(a_size.first, a_size.second);
    std::function function = [&a, &b, &a_size, &b_size, &matrix](size_t start,
                                                                 size_t end) {
      for (size_t y = start; y < end; y++) {
        for (size_t x = 0; x < a_size.first; x++) {
          T value = 0;
          for (size_t y_b = 0; y_b < b_size.second; y_b++) {
            size_t y_a = y + y_b - b_size.second / 2;
            if (y_a < a_size.second) {
              for (size_t x_b = 0; x_b < b_size.first; x_b++) {
                size_t x_a = x + x_b - b_size.first / 2;
                if (x_a < a_size.first) {
                  value += a.at(x_a, y_a) * b.at(x_b, y_b);
                }
              }
            }
          }
          matrix.at(x, y) = value;
        }
      }
    };
    ParallelRunner parallel_runner(function);
    size_t concurrency =
        std::min(std::thread::hardware_concurrency(), a_size.second);
    for (size_t i = 0; i < concurrency; i++) {
      size_t start = a_size.second * i / concurrency;
      size_t end = a_size.second * (i + 1) / concurrency;
      parallel_runner.run(start, end);
    }
    parallel_runner.wait();
    return matrix;
  }
};
