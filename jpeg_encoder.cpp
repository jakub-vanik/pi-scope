#include "jpeg_encoder.h"

#include "jpeg_stream_destination.h"

#include <jpeglib.h>

void JpegEncoder::encode(std::ostream &stream, int quality,
                         const Matrix<Pixel> &pixels)
{
  jpeg_compress_struct cinfo;
  jpeg_error_mgr jerr;
  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);
  auto size = pixels.size();
  cinfo.image_width = size.first;
  cinfo.image_height = size.second;
  cinfo.input_components = 3;
  cinfo.in_color_space = JCS_YCbCr;
  jpeg_set_defaults(&cinfo);
  jpeg_set_quality(&cinfo, quality, TRUE);
  JpegStreamDestination destination(stream, 1024 * 1024);
  cinfo.dest = &destination;
  jpeg_start_compress(&cinfo, TRUE);
  while (cinfo.next_scanline < cinfo.image_height) {
    Pixel &pixel = const_cast<Pixel &>(pixels.at(0, cinfo.next_scanline));
    JSAMPROW scanlines[1] = {reinterpret_cast<JSAMPROW>(&pixel)};
    jpeg_write_scanlines(&cinfo, scanlines, 1);
  }
  jpeg_finish_compress(&cinfo);
  jpeg_destroy_compress(&cinfo);
}
