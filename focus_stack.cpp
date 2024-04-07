#include "focus_stack.h"

#include "matrix_utils.h"

#include <cmath>

FocusStack::FocusStack(unsigned int width, unsigned int height) :
  width(width),
  height(height),
  pixels(width, height),
  best_contrast(width, height),
  gausian_kernel(MatrixUtils<short>::createGausianKernel(5, 1.0f, 16)),
  laplacian_operator(MatrixUtils<short>::createLaplacianOperator())
{
}

void FocusStack::append(const Yuv420Image &image)
{
  Matrix<short> matrix(width, height);
  for (unsigned int y = 0; y < height; y++) {
    for (unsigned int x = 0; x < width; x++) {
      matrix.at(x, y) = image.getPixel(x, y).Y;
    }
  }
  matrix = MatrixUtils<short>::convolution(matrix, gausian_kernel);
  matrix = MatrixUtils<short>::convolution(matrix, laplacian_operator);
  for (unsigned int y = 0; y < height; y++) {
    for (unsigned int x = 0; x < width; x++) {
      float pixel_contrast = std::abs(matrix.at(x, y));
      if (pixel_contrast >= best_contrast.at(x, y)) {
        pixels.at(x, y) = image.getPixel(x, y);
        best_contrast.at(x, y) = pixel_contrast;
      }
    }
  }
}

void FocusStack::encode(std::ostream &stream, int quality)
{
  JpegEncoder::encode(stream, quality, pixels);
}
