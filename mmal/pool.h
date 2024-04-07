#pragma once

#include "port.h"

#include <interface/mmal/mmal.h>

namespace mmal {

class Pool
{
public:
  Pool(Port &port);
  virtual ~Pool();
  MMAL_QUEUE_T *getQueue();

private:
  MMAL_POOL_T *pool;
};

} // namespace mmal
