#include "queue.h"

#include <chrono>
#include <exception>

namespace mmal {

Queue::Queue(Port &port, unsigned int capacity) :
  port(port),
  capacity(capacity),
  pool(port),
  queue(pool.getQueue()),
  current_frame{0, 0}
{
  port.getPort()->userdata = reinterpret_cast<MMAL_PORT_USERDATA_T *>(this);
  port.enable(&callback);
  auto length = mmal_queue_length(queue);
  for (unsigned int i = 0; i < length; i++) {
    auto *buffer = mmal_queue_get(queue);
    if (buffer) {
      port.sendBuffer(buffer);
    }
  }
}

Queue::~Queue()
{
  port.disable();
}

void Queue::reset()
{
  std::unique_lock<std::mutex> lock(mutex);
  std::queue<Frame> empty;
  std::swap(frames, empty);
}

Queue::Frame Queue::getFrame()
{
  std::unique_lock<std::mutex> lock(mutex);
  if (frames.empty()) {
    if (condition_variable.wait_for(lock, std::chrono::seconds(10)) ==
        std::cv_status::timeout) {
      throw std::exception();
    }
  }
  auto frame = std::move(frames.front());
  frames.pop();
  return frame;
}

void Queue::processBuffer(MMAL_BUFFER_HEADER_T *buffer)
{
  if (buffer->length) {
    if (mmal_buffer_header_mem_lock(buffer) != MMAL_SUCCESS) {
      throw std::exception();
    }
    current_frame.data.append(reinterpret_cast<char *>(buffer->data),
                              buffer->length);
    mmal_buffer_header_mem_unlock(buffer);
  }
  if (buffer->flags & MMAL_BUFFER_HEADER_FLAG_FRAME_END) {
    current_frame.pts = buffer->pts;
    {
      std::unique_lock<std::mutex> lock(mutex);
      frames.push(std::move(current_frame));
      if (capacity != 0 && frames.size() > capacity) {
        frames.pop();
      }
    }
    condition_variable.notify_one();
    current_frame = Frame{0, 0};
  }
  mmal_buffer_header_release(buffer);
  if (port.isEnabed()) {
    auto *buffer = mmal_queue_get(queue);
    if (buffer) {
      port.sendBuffer(buffer);
    }
  }
}

void Queue::callback(MMAL_PORT_T *port, MMAL_BUFFER_HEADER_T *buffer)
{
  try {
    Queue *self = reinterpret_cast<Queue *>(port->userdata);
    self->processBuffer(buffer);
  } catch (...) {
  }
}

} // namespace mmal
