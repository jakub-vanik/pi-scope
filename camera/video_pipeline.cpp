#include "video_pipeline.h"

namespace camera {

VideoPipeline::VideoPipeline(mmal::Port &source, unsigned int bitrate) :
  source(source), encoder(source, bitrate), output(encoder.getOutputPort(), 30)
{
}

mmal::Queue::Frame VideoPipeline::getFrame()
{
  return output.getFrame();
}

} // namespace camera
