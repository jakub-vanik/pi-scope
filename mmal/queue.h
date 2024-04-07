#pragma once

#include "pool.h"
#include "port.h"

#include <Poco/Buffer.h>
#include <condition_variable>
#include <interface/mmal/mmal.h>
#include <mutex>
#include <queue>

namespace mmal {

class Queue
{
public:
  struct Frame
  {
    Poco::Buffer<char> data;
    long long pts;
  };
  Queue(Port &port, unsigned int capacity = 0);
  virtual ~Queue();
  void reset();
  Frame getFrame();

private:
  Port &port;
  unsigned int capacity;
  Pool pool;
  MMAL_QUEUE_T *queue;
  Frame current_frame;
  std::mutex mutex;
  std::condition_variable condition_variable;
  std::queue<Frame> frames;
  void processBuffer(MMAL_BUFFER_HEADER_T *buffer);
  static void callback(MMAL_PORT_T *port, MMAL_BUFFER_HEADER_T *buffer);
};

} // namespace mmal
