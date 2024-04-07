#pragma once

#include "mmal/port.h"

#include <cstdbool>

namespace camera {

class Formatter
{
public:
  Formatter(mmal::Port &port, unsigned int width, unsigned int height,
            bool opaque);
  virtual ~Formatter();

private:
  mmal::Port &port;
  mmal::Port::Encoding encoding_backup;
  mmal::Port::Size size_backup;
};

} // namespace camera
