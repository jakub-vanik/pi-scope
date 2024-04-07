#include "preview_pipeline.h"

namespace camera {

PreviewPipeline::PreviewPipeline(mmal::Port &source, unsigned int width,
                                 unsigned int height, unsigned int quality) :
  resizer(source, width, height),
  encoder(resizer.getOutputPort()),
  output(encoder.getOutputPort(), 2)
{
  encoder.setQuality(quality);
}

mmal::Queue::Frame PreviewPipeline::getFrame()
{
  return output.getFrame();
}

} // namespace camera
