#include "resource_storage.h"

#include <Poco/StreamCopier.h>
#include <Poco/Zip/ZipStream.h>

ResourceStorage::ResourceStorage(const char *start, const char *end) :
  data(start, (end - start)), reader(data), archive(reader.stream())
{
}

bool ResourceStorage::loadResource(const std::string &name,
                                   std::ostream &stream)
{
  auto header = archive.findHeader(name);
  if (header != archive.headerEnd()) {
    reader.stream().clear();
    Poco::Zip::ZipInputStream zip_stream(reader.stream(), header->second);
    Poco::StreamCopier::copyStream(zip_stream, stream);
    return true;
  }
  return false;
}
