#include "camera_info.h"

#include <exception>
#include <interface/mmal/mmal.h>

namespace mmal {

CameraInfo::CameraInfo() : Component("vc.camera_info") {}

std::tuple<unsigned int, unsigned int> CameraInfo::getMaxSize(unsigned int id)
{
  MMAL_PARAMETER_CAMERA_INFO_T camera_info = {
      {MMAL_PARAMETER_CAMERA_INFO, sizeof(camera_info)}, 0, 0, {}, {}};
  if (mmal_port_parameter_get(component->control, &camera_info.hdr) !=
      MMAL_SUCCESS) {
    throw std::exception();
  }
  if (id >= camera_info.num_cameras) {
    throw std::exception();
  }
  return {camera_info.cameras[id].max_width,
          camera_info.cameras[id].max_height};
}

} // namespace mmal
