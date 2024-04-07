#pragma once

#include "port.h"

#include <interface/mmal/util/mmal_connection.h>

namespace mmal {

class Connection
{
public:
  Connection(Port &source, Port &destination);
  virtual ~Connection();
  void enable();
  void disable();

private:
  MMAL_CONNECTION_T *connection;
};

} // namespace mmal
