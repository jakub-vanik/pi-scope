#include "splitter.h"

#include <exception>
#include <interface/mmal/mmal.h>

namespace mmal {

Splitter::Splitter(Port &source) :
  Component("vc.splitter"),
  input_port(component->input[0]),
  output_port{component->output[0], component->output[1], component->output[2],
              component->output[3]},
  connection(source, input_port)
{
  enable();
  connection.enable();
}

Port &Splitter::getInputPort()
{
  return input_port;
}

Port &Splitter::getOutputPort(unsigned int id)
{
  return output_port[id];
}

void Splitter::pause()
{
  connection.disable();
}

void Splitter::resume()
{
  connection.enable();
}

} // namespace mmal
