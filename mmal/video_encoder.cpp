#include "video_encoder.h"

#include <exception>
#include <interface/mmal/mmal.h>

namespace mmal {

VideoEncoder::VideoEncoder(Port &source, unsigned int bitrate) :
  Component("vc.ril.video_encode"),
  input_port(component->input[0]),
  output_port(component->output[0]),
  connection(source, input_port)
{
  output_port.setEncoding({MMAL_ENCODING_H264});
  output_port.setBitrate(bitrate);
  output_port.commitFormat();
  MMAL_PARAMETER_VIDEO_PROFILE_T video_profile = {
      {MMAL_PARAMETER_PROFILE, sizeof(video_profile)}};
  video_profile.profile[0].profile = MMAL_VIDEO_PROFILE_H264_HIGH;
  video_profile.profile[0].level = MMAL_VIDEO_LEVEL_H264_4;
  if (mmal_port_parameter_set(output_port.getPort(), &video_profile.hdr) !=
      MMAL_SUCCESS) {
    throw std::exception();
  }
  enable();
  connection.enable();
}

Port &VideoEncoder::getInputPort()
{
  return input_port;
}

Port &VideoEncoder::getOutputPort()
{
  return output_port;
}

} // namespace mmal
