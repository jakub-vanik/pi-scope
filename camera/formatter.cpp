#include "formatter.h"

namespace camera {

Formatter::Formatter(mmal::Port &port, unsigned int width, unsigned int height,
                     bool opaque) :
  port(port), encoding_backup(port.getEncoding()), size_backup(port.getSize())
{
  if (opaque) {
    port.setEncoding({MMAL_ENCODING_OPAQUE, MMAL_ENCODING_I420});
  } else {
    port.setEncoding({MMAL_ENCODING_I420});
  }
  port.setSize({width, height});
  port.commitFormat();
}

Formatter::~Formatter()
{
  port.setEncoding(encoding_backup);
  port.setSize(size_backup);
  port.commitFormat();
}

} // namespace camera
