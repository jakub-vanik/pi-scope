#pragma once

#include "allocator.h"
#include "formatter.h"
#include "mmal/camera.h"
#include "mmal/splitter.h"

#include <Poco/Buffer.h>
#include <Poco/Net/MultipartWriter.h>
#include <functional>
#include <mutex>
#include <ostream>
#include <tuple>

namespace camera {

class Camera
{
public:
  Camera(unsigned int id, unsigned int preview_width,
         unsigned int preview_height, int camera_rotation);
  MMAL_PARAM_EXPOSUREMODE_T getExposureMode();
  void setExposureMode(MMAL_PARAM_EXPOSUREMODE_T exposure_mode);
  MMAL_PARAM_EXPOSUREMETERINGMODE_T getExposureMeteringMode();
  void setExposureMeteringMode(
      MMAL_PARAM_EXPOSUREMETERINGMODE_T exposure_metering_mode);
  int getExposureCompensation();
  void setExposureCompensation(int exposure_compensation);
  MMAL_PARAM_AWBMODE_T getAwbMode();
  void setAwbMode(MMAL_PARAM_AWBMODE_T awb_mode);
  std::tuple<float, float> getAwbGains();
  void setAwbGains(std::tuple<float, float> awb_gains);
  unsigned int getIso();
  void setIso(unsigned int iso);
  unsigned int getShutterSpeed();
  void setShutterSpeed(unsigned int shutter_speed);
  std::tuple<float, float, float, float> getZoom();
  void setZoom(std::tuple<float, float, float, float> zoom);
  Poco::Buffer<char> takePicture(unsigned int width, unsigned int height,
                                 unsigned int quality);
  void takePicture(std::ostream &stream, unsigned int width,
                   unsigned int height, unsigned int quality);
  Poco::Buffer<char> takeYuv420Picture(unsigned int width, unsigned int height);
  void takeYuv420Picture(std::ostream &stream, unsigned int width,
                         unsigned int height);
  void recordVideo(std::function<bool(Poco::Buffer<char>, long long)> callback,
                   unsigned int width, unsigned int height,
                   unsigned int bitrate);
  void streamPreview(Poco::Net::MultipartWriter &writer, unsigned int width,
                     unsigned int height, unsigned int quality);

private:
  std::mutex mutex;
  Allocator<4> preview_allocator;
  mmal::Camera camera;
  Formatter preview_formatter;
  mmal::Splitter preview_splitter;
};

} // namespace camera
