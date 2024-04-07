#pragma once

#include "component.h"
#include "port.h"

#include <interface/mmal/mmal.h>
#include <tuple>

namespace mmal {

class Camera : public Component
{
public:
  Camera(unsigned int id, std::tuple<unsigned int, unsigned int> max_size);
  Port &getPreviewPort();
  Port &getVideoPort();
  Port &getCapturePort();
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
  void startVideo();
  void stopVideo();
  void capture();

private:
  static const unsigned int max_video_width = 1920;
  static const unsigned int max_video_height = 1080;
  static const unsigned int preview_framerate = 4;
  static const unsigned int video_framerate = 30;
  Port preview_port;
  Port video_port;
  Port capture_port;
  void processBuffer(MMAL_BUFFER_HEADER_T *buffer);
  static void callback(MMAL_PORT_T *port, MMAL_BUFFER_HEADER_T *buffer);
};

} // namespace mmal
