#pragma once

#include "mmal/port.h"

namespace camera {

class Resizer
{
public:
  Resizer(mmal::Port &port, unsigned int width, unsigned int height);
  virtual ~Resizer();

private:
  mmal::Port &port;
  mmal::Port::Size size_backup;
};

} // namespace camera
