#pragma once

#include "matrix.h"
#include "pixel.h"

#include <ostream>

class JpegEncoder
{
public:
  static void encode(std::ostream &stream, int quality,
                     const Matrix<Pixel> &pixels);
};
