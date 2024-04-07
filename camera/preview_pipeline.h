#pragma once

#include "mmal/image_encoder.h"
#include "mmal/port.h"
#include "mmal/queue.h"
#include "mmal/resizer.h"

namespace camera {

class PreviewPipeline
{
public:
  PreviewPipeline(mmal::Port &source, unsigned int width, unsigned int height,
                  unsigned int quality);
  mmal::Queue::Frame getFrame();

private:
  mmal::Resizer resizer;
  mmal::ImageEncoder encoder;
  mmal::Queue output;
};

} // namespace camera
