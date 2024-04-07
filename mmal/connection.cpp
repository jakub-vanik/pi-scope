#include "connection.h"

#include <exception>

namespace mmal {

Connection::Connection(Port &source, Port &destination) : connection(nullptr)
{
  if (mmal_connection_create(
          &connection, source.getPort(), destination.getPort(),
          MMAL_CONNECTION_FLAG_TUNNELLING |
              MMAL_CONNECTION_FLAG_ALLOCATION_ON_INPUT) != MMAL_SUCCESS) {
    throw std::exception();
  }
}

Connection::~Connection()
{
  if (connection) {
    mmal_connection_destroy(connection);
  }
}

void Connection::enable()
{
  if (mmal_connection_enable(connection) != MMAL_SUCCESS) {
    throw std::exception();
  }
}

void Connection::disable()
{
  if (mmal_connection_disable(connection) != MMAL_SUCCESS) {
    throw std::exception();
  }
}

} // namespace mmal
