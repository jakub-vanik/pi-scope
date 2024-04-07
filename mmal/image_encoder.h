#pragma once

#include "component.h"
#include "connection.h"
#include "port.h"

namespace mmal {

class ImageEncoder : public Component
{
public:
  ImageEncoder(Port &source);
  Port &getInputPort();
  Port &getOutputPort();
  void setQuality(unsigned int quality);

private:
  Port input_port;
  Port output_port;
  Connection connection;
};

} // namespace mmal
