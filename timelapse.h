#pragma once

#include "camera/camera.h"
#include "motor.h"
#include "timer.h"

#include <cstdbool>
#include <string>

class Timelapse
{
public:
  Timelapse(camera::Camera &camera, Motor &motor);
  void startSingle(const std::string &path, unsigned int seconds,
                   unsigned int width, unsigned int height);
  void startFocusBracket(const std::string &path, unsigned int seconds,
                         unsigned int width, unsigned int height,
                         unsigned int count, int increment, int base,
                         float speed, bool stack);
  void stop();

private:
  camera::Camera &camera;
  Motor &motor;
  Timer timer;
};
