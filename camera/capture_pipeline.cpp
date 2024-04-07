#include "capture_pipeline.h"

namespace camera {

CapturePipeline::CapturePipeline(mmal::Port &source, unsigned int quality) :
  encoder(source), output(encoder.getOutputPort(), 1)
{
  encoder.setQuality(quality);
}

mmal::Queue::Frame CapturePipeline::getFrame()
{
  return output.getFrame();
}

} // namespace camera
