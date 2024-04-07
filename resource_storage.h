#pragma once

#include <Poco/BinaryReader.h>
#include <Poco/Buffer.h>
#include <Poco/Zip/ZipArchive.h>
#include <ostream>
#include <string>

class ResourceStorage
{
public:
  ResourceStorage(const char *start, const char *end);
  bool loadResource(const std::string &name, std::ostream &stream);

private:
  Poco::Buffer<char> data;
  Poco::BasicMemoryBinaryReader<char> reader;
  Poco::Zip::ZipArchive archive;
};
