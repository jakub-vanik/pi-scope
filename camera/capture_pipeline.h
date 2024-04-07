#pragma once

#include "mmal/image_encoder.h"
#include "mmal/port.h"
#include "mmal/queue.h"

namespace camera {

class CapturePipeline
{
public:
  CapturePipeline(mmal::Port &source, unsigned int quality);
  mmal::Queue::Frame getFrame();

private:
  mmal::ImageEncoder encoder;
  mmal::Queue output;
};

} // namespace camera
