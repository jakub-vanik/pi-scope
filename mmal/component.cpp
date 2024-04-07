#include "component.h"

#include <exception>

namespace mmal {

Component::Component(const char *name) : component(nullptr)
{
  if (mmal_component_create(name, &component) != MMAL_SUCCESS) {
    throw std::exception();
  }
}

Component::~Component()
{
  if (component) {
    mmal_component_destroy(component);
  }
}

void Component::enable()
{
  if (mmal_component_enable(component) != MMAL_SUCCESS) {
    throw std::exception();
  }
}

void Component::disable()
{
  if (mmal_component_disable(component) != MMAL_SUCCESS) {
    throw std::exception();
  }
}

} // namespace mmal
