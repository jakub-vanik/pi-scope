#include "yuv420_image.h"

Yuv420Image::Yuv420Image(unsigned int width, unsigned int height,
                         const Poco::Buffer<char> &&data) :
  width(width), height(height), data(data)
{
  data_width = (width + 31) / 32 * 32;
  data_height = (height + 15) / 16 * 16;
}

unsigned int Yuv420Image::getWidth() const
{
  return width;
}

unsigned int Yuv420Image::getHeight() const
{
  return height;
}

Pixel Yuv420Image::getPixel(unsigned int x, unsigned int y) const
{
  const char *y_data = data.begin();
  const char *u_data = y_data + (data_width * data_height);
  const char *v_data = u_data + (data_width / 2 * data_height / 2);
  return Pixel{y_data[y * data_width + x],
               u_data[y / 2 * data_width / 2 + x / 2],
               v_data[y / 2 * data_width / 2 + x / 2]};
}
