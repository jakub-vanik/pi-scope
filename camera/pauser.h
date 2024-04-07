#pragma once

#include "mmal/splitter.h"

namespace camera {

class Pauser
{
public:
  Pauser(mmal::Splitter &splitter);
  virtual ~Pauser();

private:
  mmal::Splitter &splitter;
};

} // namespace camera
