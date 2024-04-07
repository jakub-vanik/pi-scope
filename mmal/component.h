#pragma once

#include <interface/mmal/mmal.h>

namespace mmal {

class Component
{
public:
  Component(const char *name);
  virtual ~Component();

protected:
  MMAL_COMPONENT_T *component;
  void enable();
  void disable();
};

} // namespace mmal
