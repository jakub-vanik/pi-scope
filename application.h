#pragma once

#include "camera/camera.h"
#include "motor.h"
#include "server.h"
#include "timelapse.h"

#include <condition_variable>
#include <mutex>

class Application
{
public:
  Application();
  void run();
  void terminate();

private:
  std::mutex mutex;
  std::condition_variable condition_variable;
  camera::Camera camera;
  Motor motor;
  Timelapse timelapse;
  Server server;
};
