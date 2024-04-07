#include "motor.h"

#include <Poco/Net/SocketAddress.h>
#include <Poco/Timespan.h>
#include <exception>

Motor::Motor() : connected(false) {}

bool Motor::calibrate(bool direction, unsigned int limit)
{
  std::lock_guard<std::mutex> guard(mutex);
  try {
    ensureConnected();
    protocol::Request request;
    request.mutable_calibrate()->set_direction(direction);
    request.mutable_calibrate()->set_limit(limit);
    transmitRequest(request);
    protocol::Response response;
    receiveResponse(response);
    return response.mutable_calibrate()->success();
  } catch (...) {
    disconnect();
    throw;
  }
}

int Motor::relativeMove(int increment, float speed)
{
  std::lock_guard<std::mutex> guard(mutex);
  try {
    ensureConnected();
    protocol::Request request;
    request.mutable_relative_move()->set_increment(increment);
    request.mutable_relative_move()->set_speed(speed);
    transmitRequest(request);
    protocol::Response response;
    receiveResponse(response);
    return response.mutable_relative_move()->position();
  } catch (...) {
    disconnect();
    throw;
  }
}

void Motor::absoluteMove(int position, float speed)
{
  std::lock_guard<std::mutex> guard(mutex);
  try {
    ensureConnected();
    protocol::Request request;
    request.mutable_absolute_move()->set_position(position);
    request.mutable_absolute_move()->set_speed(speed);
    transmitRequest(request);
    protocol::Response response;
    receiveResponse(response);
  } catch (...) {
    disconnect();
    throw;
  }
}

void Motor::release()
{
  std::lock_guard<std::mutex> guard(mutex);
  try {
    ensureConnected();
    protocol::Request request;
    request.mutable_release();
    transmitRequest(request);
    protocol::Response response;
    receiveResponse(response);
  } catch (...) {
    disconnect();
    throw;
  }
}

void Motor::ensureConnected()
{
  if (!connected) {
    socket.connect(Poco::Net::SocketAddress("127.0.0.1", 1024),
                   Poco::Timespan(1, 0));
    socket.setKeepAlive(true);
    socket.setNoDelay(true);
    connected = true;
  }
}

void Motor::disconnect()
{
  socket.close();
  connected = false;
}

void Motor::transmitRequest(protocol::Request &request)
{
  unsigned int size = request.ByteSizeLong();
  if (socket.sendBytes(&size, sizeof(size), MSG_WAITALL) != sizeof(size)) {
    throw std::exception();
  }
  char data[size];
  if (!request.SerializePartialToArray(data, size)) {
    throw std::exception();
  }
  if (socket.sendBytes(data, size, MSG_WAITALL) != size) {
    throw std::exception();
  }
}

void Motor::receiveResponse(protocol::Response &response)
{
  unsigned int size;
  if (socket.receiveBytes(&size, sizeof(size), MSG_WAITALL) != sizeof(size)) {
    throw std::exception();
  }
  char data[size];
  if (socket.receiveBytes(data, size, MSG_WAITALL) != size) {
    throw std::exception();
  }
  if (!response.ParseFromArray(data, size)) {
    throw std::exception();
  }
}
