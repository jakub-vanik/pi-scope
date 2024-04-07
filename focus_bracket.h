#pragma once

#include "buffer_queue.h"
#include "camera/camera.h"
#include "motor.h"

#include <Poco/ActiveMethod.h>
#include <Poco/ActiveResult.h>
#include <cstdbool>
#include <ostream>

class FocusBracket
{
public:
  FocusBracket(camera::Camera &camera, Motor &motor, unsigned int width,
               unsigned int height, unsigned int count, int increment, int base,
               float speed, bool stack);
  virtual ~FocusBracket();
  void capture(std::ostream &stream);

private:
  camera::Camera &camera;
  Motor &motor;
  unsigned int width;
  unsigned int height;
  unsigned int count;
  int increment;
  int base;
  float speed;
  bool stack;
  BufferQueue queue;
  Poco::ActiveMethod<void, void, FocusBracket> active_method;
  Poco::ActiveResult<void> active_result;
  void activeMethod();
};
