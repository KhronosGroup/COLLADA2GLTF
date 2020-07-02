// Copyright 2020 The Khronos® Group Inc.
#pragma once

#include <string>

#include "GLTFBuffer.h"
#include "GLTFConstants.h"
#include "GLTFObject.h"

namespace GLTF {
class BufferView : public GLTF::Object {
 public:
  GLTF::Buffer* buffer = NULL;
  int byteOffset = 0;
  int byteStride = 0;
  int byteLength = 0;
  GLTF::Constants::WebGL target = (GLTF::Constants::WebGL)-1;

  BufferView(int byteOffset, int byteLength, GLTF::Buffer* buffer);
  BufferView(unsigned char* data, int dataLength);
  BufferView(unsigned char* data, int dataLength,
             GLTF::Constants::WebGL target);

  virtual std::string typeName();
  virtual void writeJSON(void* writer, GLTF::Options* options);
};
}  // namespace GLTF
