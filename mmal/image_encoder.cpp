#include "image_encoder.h"

#include <interface/mmal/mmal.h>

namespace mmal {

ImageEncoder::ImageEncoder(Port &source) :
  Component("vc.ril.image_encode"),
  input_port(component->input[0]),
  output_port(component->output[0]),
  connection(source, input_port)
{
  output_port.setEncoding({MMAL_ENCODING_JPEG});
  output_port.commitFormat();
  enable();
  connection.enable();
}

Port &ImageEncoder::getInputPort()
{
  return input_port;
}

Port &ImageEncoder::getOutputPort()
{
  return output_port;
}

void ImageEncoder::setQuality(unsigned int quality)
{
  output_port.setParameter(MMAL_PARAMETER_JPEG_Q_FACTOR, quality);
}

} // namespace mmal
