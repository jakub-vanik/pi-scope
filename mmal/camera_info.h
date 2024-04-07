#pragma once

#include "component.h"

#include <tuple>

namespace mmal {

class CameraInfo : public Component
{
public:
  CameraInfo();
  std::tuple<unsigned int, unsigned int> getMaxSize(unsigned int id);
};

} // namespace mmal
