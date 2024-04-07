#pragma once

#include "pixel.h"

#include <Poco/Buffer.h>

class Yuv420Image
{
public:
  Yuv420Image(unsigned int width, unsigned int height,
              const Poco::Buffer<char> &&data);
  unsigned int getWidth() const;
  unsigned int getHeight() const;
  Pixel getPixel(unsigned int x, unsigned int y) const;

private:
  unsigned int width;
  unsigned int height;
  Poco::Buffer<char> data;
  unsigned int data_width;
  unsigned int data_height;
};
