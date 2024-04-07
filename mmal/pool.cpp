#include "pool.h"

#include <exception>

namespace mmal {

Pool::Pool(Port &port) : pool(nullptr)
{
  pool = mmal_pool_create_with_allocator(
      port.getPort()->buffer_num, port.getPort()->buffer_size, port.getPort(),
      reinterpret_cast<mmal_pool_allocator_alloc_t>(&mmal_port_payload_alloc),
      reinterpret_cast<mmal_pool_allocator_free_t>(&mmal_port_payload_free));
  if (!pool) {
    throw std::exception();
  }
}

Pool::~Pool()
{
  if (pool) {
    mmal_pool_destroy(pool);
  }
}

MMAL_QUEUE_T *Pool::getQueue()
{
  return pool->queue;
}

} // namespace mmal
