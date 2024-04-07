#include "focus_bracket.h"

#include "focus_stack.h"
#include "jpeg_encoder.h"
#include "yuv420_image.h"

#include <Poco/BinaryReader.h>
#include <Poco/DateTime.h>
#include <Poco/Zip/Compress.h>
#include <Poco/Zip/ZipCommon.h>
#include <exception>
#include <iomanip>
#include <sstream>

FocusBracket::FocusBracket(camera::Camera &camera, Motor &motor,
                           unsigned int width, unsigned int height,
                           unsigned int count, int increment, int base,
                           float speed, bool stack) :
  camera(camera),
  motor(motor),
  width(width),
  height(height),
  count(count),
  increment(increment),
  base(base),
  speed(speed),
  stack(stack),
  queue(1),
  active_method(this, &FocusBracket::activeMethod),
  active_result(active_method())
{
}

FocusBracket::~FocusBracket()
{
  queue.terminate();
  active_result.wait();
}

void FocusBracket::capture(std::ostream &stream)
{
  if (stack) {
    FocusStack focus_stack(width, height);
    for (unsigned int i = 0; i < count; i++) {
      Yuv420Image image(width, height, queue.pull());
      focus_stack.append(image);
    }
    focus_stack.encode(stream, 100);
  } else {
    Poco::Zip::Compress archive(stream, false);
    for (unsigned int i = 0; i < count; i++) {
      int position = base + i * increment;
      auto picture = queue.pull();
      Poco::BasicMemoryBinaryReader<char> reader(picture);
      Poco::DateTime now;
      std::stringstream filename;
      filename << "images/" << std::setfill('0') << std::setw(6) << position
               << ".jpg";
      archive.addFile(reader.stream(), now, filename.str(),
                      Poco::Zip::ZipCommon::CM_STORE);
    }
    archive.close();
  }
}

void FocusBracket::activeMethod()
{
  try {
    for (unsigned int i = 0; i < count; i++) {
      int position = base + i * increment;
      motor.absoluteMove(position, speed);
      camera.setZoom({0.0f, 0.0f, 1.0f, 1.0f});
      if (stack) {
        queue.push(camera.takeYuv420Picture(width, height));
      } else {
        queue.push(camera.takePicture(width, height, 100));
      }
    }
  } catch (...) {
    queue.terminate();
  }
}
