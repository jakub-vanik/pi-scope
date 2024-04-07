#include "resizer.h"

#include <interface/mmal/mmal.h>

namespace mmal {

Resizer::Resizer(Port &source, unsigned int width, unsigned int height) :
  Component("vc.ril.resize"),
  input_port(component->input[0]),
  output_port(component->output[0]),
  connection(source, input_port)
{
  output_port.setEncoding({MMAL_ENCODING_I420});
  output_port.setSize({width, height});
  output_port.commitFormat();
  enable();
  connection.enable();
}

Port &Resizer::getInputPort()
{
  return input_port;
}

Port &Resizer::getOutputPort()
{
  return output_port;
}

} // namespace mmal
