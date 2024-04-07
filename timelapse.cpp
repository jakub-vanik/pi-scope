#include "timelapse.h"

#include "focus_bracket.h"

#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>

Timelapse::Timelapse(camera::Camera &camera, Motor &motor) :
  camera(camera), motor(motor)
{
}

void Timelapse::startSingle(const std::string &path, unsigned int seconds,
                            unsigned int width, unsigned int height)
{
  std::chrono::seconds interval(seconds);
  timer.start(interval, [this, path, width, height]() {
    try {
      std::time_t now = std::time(nullptr);
      std::stringstream filename;
      filename << std::put_time(std::localtime(&now), "%Y%m%d_%H%M%S")
               << ".jpg";
      std::filesystem::path storage_path(path);
      std::ofstream file(storage_path / filename.str());
      this->camera.setZoom({0.0f, 0.0f, 1.0f, 1.0f});
      this->camera.takePicture(file, width, height, 100);
      file.close();
    } catch (...) {
    }
  });
}

void Timelapse::startFocusBracket(const std::string &path, unsigned int seconds,
                                  unsigned int width, unsigned int height,
                                  unsigned int count, int increment, int base,
                                  float speed, bool stack)
{
  std::chrono::seconds interval(seconds);
  timer.start(interval, [this, path, width, height, count, increment, base,
                         speed, stack]() {
    try {
      std::time_t now = std::time(nullptr);
      std::stringstream filename;
      filename << std::put_time(std::localtime(&now), "%Y%m%d_%H%M%S");
      if (stack) {
        filename << ".jpg";
      } else {
        filename << ".zip";
      }
      std::filesystem::path storage_path(path);
      std::ofstream file(storage_path / filename.str());
      FocusBracket focus_bracket(this->camera, this->motor, width, height,
                                 count, increment, base, speed, stack);
      focus_bracket.capture(file);
      file.close();
    } catch (...) {
    }
  });
}

void Timelapse::stop()
{
  timer.stop();
}
