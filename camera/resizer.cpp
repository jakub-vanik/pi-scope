#include "resizer.h"

namespace camera {

Resizer::Resizer(mmal::Port &port, unsigned int width, unsigned int height) :
  port(port), size_backup(port.getSize())
{
  port.setSize({width, height});
  port.commitFormat();
}

Resizer::~Resizer()
{
  port.setSize(size_backup);
  port.commitFormat();
}

} // namespace camera
