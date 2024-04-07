#pragma once

#include "atomic_signal.h"
#include "timer.h"

#include <Poco/AutoPtr.h>
#include <Poco/Util/XMLConfiguration.h>
#include <cstdbool>
#include <mutex>
#include <string>
#include <thread>

class ConfigStorage
{
public:
  struct CameraSettings
  {
    unsigned int exposure_mode;
    unsigned int exposure_metering_mode;
    int exposure_compensation;
    unsigned int awb_mode;
    float awb_gain_red;
    float awb_gain_blue;
    unsigned int iso;
    unsigned int shutter_speed;
  };
  struct TimelapseSettings
  {
    std::string path;
    unsigned int seconds;
    unsigned int width;
    unsigned int height;
    unsigned int mode;
    unsigned int count;
    int increment;
    int base;
    float speed;
    bool stack;
  };
  ConfigStorage(const std::string &path);
  virtual ~ConfigStorage();
  CameraSettings getCameraSettings();
  void setCameraSettings(const CameraSettings &camera_settings);
  TimelapseSettings getTimelapseSettings();
  void setTimelapseSettings(const TimelapseSettings &timelapse_settings);

private:
  std::string path;
  std::mutex mutex;
  Timer save_timer;
  AtomicSignal save_signal;
  Poco::AutoPtr<Poco::Util::XMLConfiguration> configuration;
  void load();
  void save();
};
