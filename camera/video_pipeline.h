#pragma once

#include "mmal/port.h"
#include "mmal/queue.h"
#include "mmal/video_encoder.h"

namespace camera {

class VideoPipeline
{
public:
  VideoPipeline(mmal::Port &source, unsigned int bitrate);
  mmal::Queue::Frame getFrame();

private:
  mmal::Port &source;
  mmal::VideoEncoder encoder;
  mmal::Queue output;
};

} // namespace camera
