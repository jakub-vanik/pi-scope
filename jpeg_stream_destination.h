#pragma once

#include <Poco/Buffer.h>
#include <cstddef>
#include <jpeglib.h>
#include <ostream>

class JpegStreamDestination : public jpeg_destination_mgr
{
public:
  JpegStreamDestination(std::ostream &stream, size_t buffer_size);

private:
  std::ostream &stream;
  Poco::Buffer<JOCTET> buffer;
  static void initDestination(j_compress_ptr cinfo);
  static boolean emptyOutputBuffer(j_compress_ptr cinfo);
  static void termDestination(j_compress_ptr cinfo);
};
