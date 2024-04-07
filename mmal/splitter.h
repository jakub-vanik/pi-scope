#pragma once

#include "component.h"
#include "connection.h"
#include "port.h"

namespace mmal {

class Splitter : public Component
{
public:
  Splitter(Port &source);
  Port &getInputPort();
  Port &getOutputPort(unsigned int id);
  void pause();
  void resume();

private:
  Port input_port;
  Port output_port[4];
  Connection connection;
};

} // namespace mmal
