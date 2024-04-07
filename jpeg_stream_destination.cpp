#include "jpeg_stream_destination.h"

JpegStreamDestination::JpegStreamDestination(std::ostream &stream,
                                             size_t buffer_size) :
  stream(stream), buffer(buffer_size)
{
  init_destination = &initDestination;
  empty_output_buffer = &emptyOutputBuffer;
  term_destination = &termDestination;
};

void JpegStreamDestination::initDestination(j_compress_ptr cinfo)
{
  JpegStreamDestination *destination =
      reinterpret_cast<JpegStreamDestination *>(cinfo->dest);
  destination->next_output_byte = destination->buffer.begin();
  destination->free_in_buffer = destination->buffer.sizeBytes();
};

boolean JpegStreamDestination::emptyOutputBuffer(j_compress_ptr cinfo)
{
  JpegStreamDestination *destination =
      reinterpret_cast<JpegStreamDestination *>(cinfo->dest);
  destination->stream.write(
      reinterpret_cast<char *>(destination->buffer.begin()),
      destination->buffer.sizeBytes());
  destination->next_output_byte = destination->buffer.begin();
  destination->free_in_buffer = destination->buffer.sizeBytes();
  return TRUE;
};

void JpegStreamDestination::termDestination(j_compress_ptr cinfo)
{
  JpegStreamDestination *destination =
      reinterpret_cast<JpegStreamDestination *>(cinfo->dest);
  destination->stream.write(
      reinterpret_cast<char *>(destination->buffer.begin()),
      destination->buffer.sizeBytes() - destination->free_in_buffer);
};
