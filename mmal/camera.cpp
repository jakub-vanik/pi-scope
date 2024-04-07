#include "camera.h"

#include <cstdbool>
#include <exception>

namespace mmal {

Camera::Camera(unsigned int id,
               std::tuple<unsigned int, unsigned int> max_size) :
  Component("vc.ril.camera"),
  preview_port(component->output[0]),
  video_port(component->output[1]),
  capture_port(component->output[2])
{
  unsigned int capture_width = std::get<0>(max_size);
  unsigned int capture_height = std::get<1>(max_size);
  unsigned int video_width =
      (capture_width > max_video_width) ? max_video_width : capture_width;
  unsigned int video_height =
      (capture_height > max_video_height) ? max_video_height : capture_height;
  component->control->userdata = reinterpret_cast<MMAL_PORT_USERDATA_T *>(this);
  if (mmal_port_enable(component->control, &callback) != MMAL_SUCCESS) {
    throw std::exception();
  }
  MMAL_PARAMETER_INT32_T camera_num = {
      {MMAL_PARAMETER_CAMERA_NUM, sizeof(camera_num)}, int(id)};
  if (mmal_port_parameter_set(component->control, &camera_num.hdr) !=
      MMAL_SUCCESS) {
    throw std::exception();
  }
  MMAL_PARAMETER_CAMERA_CONFIG_T camera_config = {
      {MMAL_PARAMETER_CAMERA_CONFIG, sizeof(camera_config)},
      .max_stills_w = capture_width,
      .max_stills_h = capture_height,
      .stills_yuv422 = 0,
      .one_shot_stills = 1,
      .max_preview_video_w = video_width,
      .max_preview_video_h = video_height,
      .num_preview_video_frames = 3,
      .stills_capture_circular_buffer_height = 0,
      .fast_preview_resume = 0,
      .use_stc_timestamp = MMAL_PARAM_TIMESTAMP_MODE_RESET_STC};
  if (mmal_port_parameter_set(component->control, &camera_config.hdr) !=
      MMAL_SUCCESS) {
    throw std::exception();
  }
  preview_port.setEncoding({MMAL_ENCODING_OPAQUE, MMAL_ENCODING_I420});
  preview_port.setSize({video_width, video_height});
  preview_port.setFramerate({preview_framerate, 1});
  preview_port.commitFormat();
  video_port.setEncoding({MMAL_ENCODING_OPAQUE, MMAL_ENCODING_I420});
  video_port.setSize({video_width, video_height});
  video_port.setFramerate({video_framerate, 1});
  video_port.commitFormat();
  capture_port.setEncoding({MMAL_ENCODING_OPAQUE, MMAL_ENCODING_I420});
  capture_port.setSize({capture_width, capture_height});
  capture_port.setFramerate({0, 1});
  capture_port.commitFormat();
  enable();
}

Port &Camera::getPreviewPort()
{
  return preview_port;
}

Port &Camera::getVideoPort()
{
  return video_port;
}

Port &Camera::getCapturePort()
{
  return capture_port;
}

MMAL_PARAM_EXPOSUREMODE_T Camera::getExposureMode()
{
  MMAL_PARAMETER_EXPOSUREMODE_T parameter = {
      {MMAL_PARAMETER_EXPOSURE_MODE, sizeof(parameter)},
      MMAL_PARAM_EXPOSUREMODE_T(0)};
  if (mmal_port_parameter_get(component->control, &parameter.hdr) !=
      MMAL_SUCCESS) {
    throw std::exception();
  }
  return parameter.value;
}

void Camera::setExposureMode(MMAL_PARAM_EXPOSUREMODE_T exposure_mode)
{
  MMAL_PARAMETER_EXPOSUREMODE_T parameter = {
      {MMAL_PARAMETER_EXPOSURE_MODE, sizeof(parameter)}, exposure_mode};
  if (mmal_port_parameter_set(component->control, &parameter.hdr) !=
      MMAL_SUCCESS) {
    throw std::exception();
  }
}

MMAL_PARAM_EXPOSUREMETERINGMODE_T Camera::getExposureMeteringMode()
{
  MMAL_PARAMETER_EXPOSUREMETERINGMODE_T parameter = {
      {MMAL_PARAMETER_EXP_METERING_MODE, sizeof(parameter)},
      MMAL_PARAM_EXPOSUREMETERINGMODE_T(0)};
  if (mmal_port_parameter_get(component->control, &parameter.hdr) !=
      MMAL_SUCCESS) {
    throw std::exception();
  }
  return parameter.value;
}

void Camera::setExposureMeteringMode(
    MMAL_PARAM_EXPOSUREMETERINGMODE_T exposure_metering_mode)
{
  MMAL_PARAMETER_EXPOSUREMETERINGMODE_T parameter = {
      {MMAL_PARAMETER_EXP_METERING_MODE, sizeof(parameter)},
      exposure_metering_mode};
  if (mmal_port_parameter_set(component->control, &parameter.hdr) !=
      MMAL_SUCCESS) {
    throw std::exception();
  }
}

int Camera::getExposureCompensation()
{
  MMAL_PARAMETER_INT32_T parameter = {
      {MMAL_PARAMETER_EXPOSURE_COMP, sizeof(parameter)}, 0};
  if (mmal_port_parameter_get(component->control, &parameter.hdr) !=
      MMAL_SUCCESS) {
    throw std::exception();
  }
  return parameter.value;
}

void Camera::setExposureCompensation(int exposure_compensation)
{
  MMAL_PARAMETER_INT32_T parameter = {
      {MMAL_PARAMETER_EXPOSURE_COMP, sizeof(parameter)}, exposure_compensation};
  if (mmal_port_parameter_set(component->control, &parameter.hdr) !=
      MMAL_SUCCESS) {
    throw std::exception();
  }
}

MMAL_PARAM_AWBMODE_T Camera::getAwbMode()
{
  MMAL_PARAMETER_AWBMODE_T parameter = {
      {MMAL_PARAMETER_AWB_MODE, sizeof(parameter)}, MMAL_PARAM_AWBMODE_T(0)};
  if (mmal_port_parameter_get(component->control, &parameter.hdr) !=
      MMAL_SUCCESS) {
    throw std::exception();
  }
  return parameter.value;
}

void Camera::setAwbMode(MMAL_PARAM_AWBMODE_T awb_mode)
{
  MMAL_PARAMETER_AWBMODE_T parameter = {
      {MMAL_PARAMETER_AWB_MODE, sizeof(parameter)}, awb_mode};
  if (mmal_port_parameter_set(component->control, &parameter.hdr) !=
      MMAL_SUCCESS) {
    throw std::exception();
  }
}

std::tuple<float, float> Camera::getAwbGains()
{
  MMAL_PARAMETER_AWB_GAINS_T parameter = {
      {MMAL_PARAMETER_CUSTOM_AWB_GAINS, sizeof(parameter)}, {0, 0}, {0, 0}};
  if (mmal_port_parameter_get(component->control, &parameter.hdr) !=
      MMAL_SUCCESS) {
    throw std::exception();
  }
  return std::make_tuple(
      float(parameter.r_gain.num) / float(parameter.r_gain.den),
      float(parameter.b_gain.num) / float(parameter.b_gain.den));
}

void Camera::setAwbGains(std::tuple<float, float> awb_gains)
{
  MMAL_PARAMETER_AWB_GAINS_T parameter = {
      {MMAL_PARAMETER_CUSTOM_AWB_GAINS, sizeof(parameter)},
      {int(std::get<0>(awb_gains) * 65536), 65536},
      {int(std::get<1>(awb_gains) * 65536), 65536}};
  if (mmal_port_parameter_set(component->control, &parameter.hdr) !=
      MMAL_SUCCESS) {
    throw std::exception();
  }
}

unsigned int Camera::getIso()
{
  MMAL_PARAMETER_UINT32_T parameter = {{MMAL_PARAMETER_ISO, sizeof(parameter)},
                                       0};
  if (mmal_port_parameter_get(component->control, &parameter.hdr) !=
      MMAL_SUCCESS) {
    throw std::exception();
  }
  return parameter.value;
}

void Camera::setIso(unsigned int iso)
{
  MMAL_PARAMETER_UINT32_T parameter = {{MMAL_PARAMETER_ISO, sizeof(parameter)},
                                       iso};
  if (mmal_port_parameter_set(component->control, &parameter.hdr) !=
      MMAL_SUCCESS) {
    throw std::exception();
  }
}

unsigned int Camera::getShutterSpeed()
{
  MMAL_PARAMETER_UINT32_T parameter = {
      {MMAL_PARAMETER_SHUTTER_SPEED, sizeof(parameter)}, 0};
  if (mmal_port_parameter_get(component->control, &parameter.hdr) !=
      MMAL_SUCCESS) {
    throw std::exception();
  }
  return parameter.value;
}

void Camera::setShutterSpeed(unsigned int shutter_speed)
{
  MMAL_PARAMETER_UINT32_T parameter = {
      {MMAL_PARAMETER_SHUTTER_SPEED, sizeof(parameter)}, shutter_speed};
  if (mmal_port_parameter_set(component->control, &parameter.hdr) !=
      MMAL_SUCCESS) {
    throw std::exception();
  }
}

std::tuple<float, float, float, float> Camera::getZoom()
{
  MMAL_PARAMETER_INPUT_CROP_T parameter = {
      {MMAL_PARAMETER_INPUT_CROP, sizeof(parameter)}, {0, 0, 0, 0}};
  if (mmal_port_parameter_get(component->control, &parameter.hdr) !=
      MMAL_SUCCESS) {
    throw std::exception();
  }
  return std::make_tuple(float(parameter.rect.x) / 65535,
                         float(parameter.rect.y) / 65535,
                         float(parameter.rect.width) / 65535,
                         float(parameter.rect.height) / 65535);
}

void Camera::setZoom(std::tuple<float, float, float, float> zoom)
{
  MMAL_PARAMETER_INPUT_CROP_T parameter = {
      {MMAL_PARAMETER_INPUT_CROP, sizeof(parameter)},
      {int(std::get<0>(zoom) * 65535), int(std::get<1>(zoom) * 65535),
       int(std::get<2>(zoom) * 65535), int(std::get<3>(zoom) * 65535)}};
  if (mmal_port_parameter_set(component->control, &parameter.hdr) !=
      MMAL_SUCCESS) {
    throw std::exception();
  }
}

void Camera::startVideo()
{
  video_port.setParameter(MMAL_PARAMETER_CAPTURE, true);
}

void Camera::stopVideo()
{
  video_port.setParameter(MMAL_PARAMETER_CAPTURE, false);
}

void Camera::capture()
{
  capture_port.setParameter(MMAL_PARAMETER_CAPTURE, true);
}

void Camera::processBuffer(MMAL_BUFFER_HEADER_T *buffer)
{
  mmal_buffer_header_release(buffer);
}

void Camera::callback(MMAL_PORT_T *port, MMAL_BUFFER_HEADER_T *buffer)
{
  Camera *self = reinterpret_cast<Camera *>(port->userdata);
  self->processBuffer(buffer);
}

} // namespace mmal
