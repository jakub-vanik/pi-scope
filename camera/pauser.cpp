#include "pauser.h"

namespace camera {

Pauser::Pauser(mmal::Splitter &splitter) : splitter(splitter)
{
  splitter.pause();
}

Pauser::~Pauser()
{
  splitter.resume();
}

} // namespace camera
