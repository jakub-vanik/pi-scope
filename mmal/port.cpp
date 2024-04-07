#include "port.h"

#include <algorithm>
#include <exception>

namespace mmal {

Port::Port(MMAL_PORT_T *port) : port(port) {}

MMAL_PORT_T *Port::getPort()
{
  return port;
}

Port::Encoding Port::getEncoding()
{
  return Encoding{port->format->encoding, port->format->encoding_variant};
}

void Port::setEncoding(Port::Encoding encoding)
{
  port->format->encoding = encoding.encoding;
  port->format->encoding_variant = encoding.variant;
}

unsigned int Port::getBitrate()
{
  return port->format->bitrate;
}

void Port::setBitrate(unsigned int bitrate)
{
  port->format->bitrate = bitrate;
}

Port::Size Port::getSize()
{
  return Size{(unsigned int) (port->format->es->video.crop.width),
              (unsigned int) (port->format->es->video.crop.height)};
}

void Port::setSize(Port::Size size)
{
  port->format->es->video.width = size.width;
  port->format->es->video.height = size.height + (-size.height % 16);
  port->format->es->video.crop.x = 0;
  port->format->es->video.crop.y = 0;
  port->format->es->video.crop.width = size.width;
  port->format->es->video.crop.height = size.height;
}

Port::Framerate Port::getFramerate()
{
  return Framerate{port->format->es->video.frame_rate.num,
                   port->format->es->video.frame_rate.den};
}

void Port::setFramerate(Port::Framerate framerate)
{
  port->format->es->video.frame_rate.num = framerate.numerator;
  port->format->es->video.frame_rate.den = framerate.denominator;
}

void Port::commitFormat()
{
  if (mmal_port_format_commit(port) != MMAL_SUCCESS) {
    throw std::exception();
  }
  port->buffer_num =
      std::max(port->buffer_num_min, port->buffer_num_recommended);
  port->buffer_size =
      std::max(port->buffer_size_min, port->buffer_size_recommended);
}

void Port::setParameter(unsigned int id, bool value)
{
  MMAL_PARAMETER_BOOLEAN_T parameter = {{id, sizeof(parameter)}, value};
  if (mmal_port_parameter_set(port, &parameter.hdr) != MMAL_SUCCESS) {
    throw std::exception();
  }
}

void Port::setParameter(unsigned int id, int value)
{
  MMAL_PARAMETER_INT32_T parameter = {{id, sizeof(parameter)}, value};
  if (mmal_port_parameter_set(port, &parameter.hdr) != MMAL_SUCCESS) {
    throw std::exception();
  }
}

void Port::setParameter(unsigned int id, unsigned int value)
{
  MMAL_PARAMETER_UINT32_T parameter = {{id, sizeof(parameter)}, value};
  if (mmal_port_parameter_set(port, &parameter.hdr) != MMAL_SUCCESS) {
    throw std::exception();
  }
}

bool Port::isEnabed()
{
  return port->is_enabled;
}

void Port::enable(MMAL_PORT_BH_CB_T callback)
{
  if (mmal_port_enable(port, callback) != MMAL_SUCCESS) {
    throw std::exception();
  }
}

void Port::disable()
{
  if (mmal_port_disable(port) != MMAL_SUCCESS) {
    throw std::exception();
  }
}

void Port::sendBuffer(MMAL_BUFFER_HEADER_T *buffer)
{
  if (mmal_port_send_buffer(port, buffer) != MMAL_SUCCESS) {
    throw std::exception();
  }
}

} // namespace mmal
