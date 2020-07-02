// Copyright 2020 The Khronos® Group Inc.
#pragma once

#include <string>

#include "GLTFObject.h"

namespace GLTF {
class Buffer : public GLTF::Object {
 public:
  unsigned char* data = NULL;
  int byteLength;
  std::string uri;

  Buffer(unsigned char* data, int dataLength);
  virtual ~Buffer();

  virtual std::string typeName();
  virtual void writeJSON(void* writer, GLTF::Options* options);
};
}  // namespace GLTF
