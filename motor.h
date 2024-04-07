#pragma once

#include "protocol.pb.h"

#include <Poco/Net/StreamSocket.h>
#include <cstdbool>
#include <mutex>

class Motor
{
public:
  Motor();
  bool calibrate(bool direction, unsigned int limit);
  int relativeMove(int increment, float speed = 1.0f);
  void absoluteMove(int position, float speed = 1.0f);
  void release();

private:
  std::mutex mutex;
  Poco::Net::StreamSocket socket;
  bool connected;
  void ensureConnected();
  void disconnect();
  void transmitRequest(protocol::Request &request);
  void receiveResponse(protocol::Response &response);
};
