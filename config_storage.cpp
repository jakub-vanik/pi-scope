#include "config_storage.h"

#include <Poco/Util/XMLConfiguration.h>
#include <chrono>

ConfigStorage::ConfigStorage(const std::string &path) :
  path(path), configuration(new Poco::Util::XMLConfiguration())
{
  try {
    load();
  } catch (...) {
  }
  save_timer.start(std::chrono::minutes(1), [this]() {
    if (save_signal.check()) {
      try {
        save();
      } catch (...) {
      }
    }
  });
}

ConfigStorage::~ConfigStorage()
{
  save_timer.stop();
  if (save_signal.check()) {
    try {
      save();
    } catch (...) {
    }
  }
}

ConfigStorage::CameraSettings ConfigStorage::getCameraSettings()
{
  std::unique_lock<std::mutex> lock(mutex);
  return CameraSettings{
      .exposure_mode = configuration->getUInt("camera[@exposure_mode]", 1),
      .exposure_metering_mode =
          configuration->getUInt("camera[@exposure_metering_mode]", 0),
      .exposure_compensation =
          configuration->getInt("camera[@exposure_compensation]", 0),
      .awb_mode = configuration->getUInt("camera[@awb_mode]", 1),
      .awb_gain_red =
          float(configuration->getDouble("camera[@awb_gain_red]", 0.0)),
      .awb_gain_blue =
          float(configuration->getDouble("camera[@awb_gain_blue]", 0.0)),
      .iso = configuration->getUInt("camera[@iso]", 0),
      .shutter_speed = configuration->getUInt("camera[@shutter_speed]", 0)};
}

void ConfigStorage::setCameraSettings(
    const ConfigStorage::CameraSettings &camera_settings)
{
  std::unique_lock<std::mutex> lock(mutex);
  configuration->setUInt("camera[@exposure_mode]",
                         camera_settings.exposure_mode);
  configuration->setUInt("camera[@exposure_metering_mode]",
                         camera_settings.exposure_metering_mode);
  configuration->setInt("camera[@exposure_compensation]",
                        camera_settings.exposure_compensation);
  configuration->setUInt("camera[@awb_mode]", camera_settings.awb_mode);
  configuration->setDouble("camera[@awb_gain_red]",
                           camera_settings.awb_gain_red);
  configuration->setDouble("camera[@awb_gain_blue]",
                           camera_settings.awb_gain_blue);
  configuration->setUInt("camera[@iso]", camera_settings.iso);
  configuration->setUInt("camera[@shutter_speed]",
                         camera_settings.shutter_speed);
  save_signal.set();
}

ConfigStorage::TimelapseSettings ConfigStorage::getTimelapseSettings()
{
  std::unique_lock<std::mutex> lock(mutex);
  return TimelapseSettings{
      .path = configuration->getString("timelapse[@path]", ""),
      .seconds = configuration->getUInt("timelapse[@seconds]", 60),
      .width = configuration->getUInt("timelapse[@width]", 4056),
      .height = configuration->getUInt("timelapse[@height]", 3040),
      .mode = configuration->getUInt("timelapse[@mode]", 0),
      .count = configuration->getUInt("timelapse[@count]", 10),
      .increment = configuration->getInt("timelapse[@increment]", 100),
      .base = configuration->getInt("timelapse[@base]", 30000),
      .speed = float(configuration->getDouble("timelapse[@speed]", 1.0)),
      .stack = configuration->getBool("timelapse[@stack]", false)};
}

void ConfigStorage::setTimelapseSettings(
    const ConfigStorage::TimelapseSettings &timelapse_settings)
{
  std::unique_lock<std::mutex> lock(mutex);
  configuration->setString("timelapse[@path]", timelapse_settings.path);
  configuration->setUInt("timelapse[@seconds]", timelapse_settings.seconds);
  configuration->setUInt("timelapse[@width]", timelapse_settings.width);
  configuration->setUInt("timelapse[@height]", timelapse_settings.height);
  configuration->setUInt("timelapse[@mode]", timelapse_settings.mode);
  configuration->setUInt("timelapse[@count]", timelapse_settings.count);
  configuration->setInt("timelapse[@increment]", timelapse_settings.increment);
  configuration->setInt("timelapse[@base]", timelapse_settings.base);
  configuration->setDouble("timelapse[@speed]", timelapse_settings.speed);
  configuration->setBool("timelapse[@stack]", timelapse_settings.stack);
  save_signal.set();
}

void ConfigStorage::load()
{
  std::unique_lock<std::mutex> lock(mutex);
  configuration->load(path);
}

void ConfigStorage::save()
{
  std::unique_lock<std::mutex> lock(mutex);
  configuration->save(path);
}
