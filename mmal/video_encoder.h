#pragma once

#include "component.h"
#include "connection.h"
#include "port.h"

namespace mmal {

class VideoEncoder : public Component
{
public:
  VideoEncoder(Port &source, unsigned int bitrate);
  Port &getInputPort();
  Port &getOutputPort();

private:
  Port input_port;
  Port output_port;
  Connection connection;
};

} // namespace mmal
