#pragma once

#include "component.h"
#include "connection.h"
#include "port.h"

namespace mmal {

class Resizer : public Component
{
public:
  Resizer(Port &source, unsigned int width, unsigned int height);
  Port &getInputPort();
  Port &getOutputPort();

private:
  Port input_port;
  Port output_port;
  Connection connection;
};

} // namespace mmal
