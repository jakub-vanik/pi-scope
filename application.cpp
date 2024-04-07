#include "application.h"

#include <Poco/ThreadPool.h>
#include <iostream>

Application::Application() :
  camera(0, 800, 600, 180),
  timelapse(camera, motor),
  server(camera, motor, timelapse)
{
}

void Application::run()
{
  std::unique_lock<std::mutex> lock(mutex);
  std::cout << "starting" << std::endl;
  server.start();
  std::cout << "started" << std::endl;
  condition_variable.wait(lock);
  std::cout << "terminating" << std::endl;
  server.stop();
  Poco::ThreadPool::defaultPool().joinAll();
  std::cout << "terminated" << std::endl;
}

void Application::terminate()
{
  condition_variable.notify_one();
}
