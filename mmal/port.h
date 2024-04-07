#pragma once

#include <cstdbool>
#include <interface/mmal/mmal.h>

namespace mmal {

class Port
{
public:
  struct Encoding
  {
    MMAL_FOURCC_T encoding;
    MMAL_FOURCC_T variant;
  };
  struct Size
  {
    unsigned int width;
    unsigned int height;
  };
  struct Framerate
  {
    int numerator;
    int denominator;
  };
  Port(MMAL_PORT_T *port);
  MMAL_PORT_T *getPort();
  Encoding getEncoding();
  void setEncoding(Encoding encoding);
  unsigned int getBitrate();
  void setBitrate(unsigned int bitrate);
  Size getSize();
  void setSize(Size size);
  Framerate getFramerate();
  void setFramerate(Framerate framerate);
  void commitFormat();
  void setParameter(unsigned int id, bool value);
  void setParameter(unsigned int id, int value);
  void setParameter(unsigned int id, unsigned int value);
  bool isEnabed();
  void enable(MMAL_PORT_BH_CB_T callback);
  void disable();
  void sendBuffer(MMAL_BUFFER_HEADER_T *buffer);

private:
  MMAL_PORT_T *port;
};

} // namespace mmal
