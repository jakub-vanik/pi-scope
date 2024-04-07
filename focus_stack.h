#pragma once

#include "jpeg_encoder.h"
#include "matrix.h"
#include "pixel.h"
#include "yuv420_image.h"

class FocusStack
{
public:
  FocusStack(unsigned int width, unsigned int height);
  void append(const Yuv420Image &image);
  void encode(std::ostream &stream, int quality);

private:
  unsigned int width;
  unsigned int height;
  Matrix<Pixel> pixels;
  Matrix<short> best_contrast;
  Matrix<short> gausian_kernel;
  Matrix<short> laplacian_operator;
};
