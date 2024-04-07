#include "camera.h"

#include "capture_pipeline.h"
#include "mmal/camera_info.h"
#include "mmal/queue.h"
#include "pauser.h"
#include "preview_pipeline.h"
#include "video_pipeline.h"

#include <Poco/Net/MessageHeader.h>
#include <exception>
#include <string>

namespace camera {

Camera::Camera(unsigned int id, unsigned int preview_width,
               unsigned int preview_height, int camera_rotation) :
  camera(id, mmal::CameraInfo().getMaxSize(id)),
  preview_formatter(camera.getPreviewPort(), preview_width, preview_height,
                    true),
  preview_splitter(camera.getPreviewPort())
{
  camera.getPreviewPort().setParameter(MMAL_PARAMETER_ROTATION,
                                       camera_rotation);
  camera.getVideoPort().setParameter(MMAL_PARAMETER_ROTATION, camera_rotation);
  camera.getCapturePort().setParameter(MMAL_PARAMETER_ROTATION,
                                       camera_rotation);
}

MMAL_PARAM_EXPOSUREMODE_T Camera::getExposureMode()
{
  return camera.getExposureMode();
}

void Camera::setExposureMode(MMAL_PARAM_EXPOSUREMODE_T exposure_mode)
{
  camera.setExposureMode(exposure_mode);
}

MMAL_PARAM_EXPOSUREMETERINGMODE_T Camera::getExposureMeteringMode()
{
  return camera.getExposureMeteringMode();
}

void Camera::setExposureMeteringMode(
    MMAL_PARAM_EXPOSUREMETERINGMODE_T exposure_metering_mode)
{
  camera.setExposureMeteringMode(exposure_metering_mode);
}

int Camera::getExposureCompensation()
{
  return camera.getExposureCompensation();
}

void Camera::setExposureCompensation(int exposure_compensation)
{
  camera.setExposureCompensation(exposure_compensation);
}

MMAL_PARAM_AWBMODE_T Camera::getAwbMode()
{
  return camera.getAwbMode();
}

void Camera::setAwbMode(MMAL_PARAM_AWBMODE_T awb_mode)
{
  camera.setAwbMode(awb_mode);
}

std::tuple<float, float> Camera::getAwbGains()
{
  return camera.getAwbGains();
}

void Camera::setAwbGains(std::tuple<float, float> awb_gains)
{
  camera.setAwbGains(awb_gains);
}

unsigned int Camera::getIso()
{
  return camera.getIso();
}

void Camera::setIso(unsigned int iso)
{
  camera.setIso(iso);
}

unsigned int Camera::getShutterSpeed()
{
  return camera.getShutterSpeed();
}

void Camera::setShutterSpeed(unsigned int shutter_speed)
{
  camera.setShutterSpeed(shutter_speed);
}

std::tuple<float, float, float, float> Camera::getZoom()
{
  return camera.getZoom();
}

void Camera::setZoom(std::tuple<float, float, float, float> zoom)
{
  camera.setZoom(zoom);
}

Poco::Buffer<char> Camera::takePicture(unsigned int width, unsigned int height,
                                       unsigned int quality)
{
  std::unique_lock<std::mutex> lock(mutex);
  auto &port = camera.getCapturePort();
  Formatter formatter(port, width, height, true);
  CapturePipeline pipeline(port, quality);
  camera.capture();
  auto frame = pipeline.getFrame();
  return std::move(frame.data);
}

void Camera::takePicture(std::ostream &stream, unsigned int width,
                         unsigned int height, unsigned int quality)
{
  auto data = takePicture(width, height, quality);
  stream.write(data.begin(), data.sizeBytes());
}

Poco::Buffer<char> Camera::takeYuv420Picture(unsigned int width,
                                             unsigned int height)
{
  std::unique_lock<std::mutex> lock(mutex);
  auto &port = camera.getCapturePort();
  Formatter formatter(port, width, height, false);
  mmal::Queue output(port, 1);
  camera.capture();
  auto frame = output.getFrame();
  return std::move(frame.data);
}

void Camera::takeYuv420Picture(std::ostream &stream, unsigned int width,
                               unsigned int height)
{
  auto data = takeYuv420Picture(width, height);
  stream.write(data.begin(), data.sizeBytes());
}

void Camera::recordVideo(
    std::function<bool(Poco::Buffer<char>, long long pts)> callback,
    unsigned int width, unsigned int height, unsigned int bitrate)
{
  std::unique_lock<std::mutex> lock(mutex);
  Pauser pauser(preview_splitter);
  auto &port = camera.getVideoPort();
  Formatter formatter(port, width, height, true);
  VideoPipeline pipeline(port, bitrate);
  camera.startVideo();
  while (true) {
    auto frame = pipeline.getFrame();
    if (!callback(frame.data, frame.pts)) {
      break;
    }
  }
  camera.stopVideo();
}

void Camera::streamPreview(Poco::Net::MultipartWriter &writer,
                           unsigned int width, unsigned int height,
                           unsigned int quality)
{
  auto token = preview_allocator.allocate();
  auto &port = preview_splitter.getOutputPort(token.getId());
  PreviewPipeline pipeline(port, width, height, quality);
  std::ostream &stream = writer.stream();
  while (stream.good()) {
    auto frame = pipeline.getFrame();
    Poco::Net::MessageHeader header;
    header.add("Content-Type", "image/jpeg");
    header.add("Content-Length", std::to_string(frame.data.sizeBytes()));
    writer.nextPart(header);
    stream.write(frame.data.begin(), frame.data.sizeBytes());
    stream.flush();
  }
}

} // namespace camera
