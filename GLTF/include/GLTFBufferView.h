#pragma once

#include "GLTFBuffer.h"
#include "GLTFConstants.h"
#include "GLTFObject.h"

namespace GLTF {
  class BufferView : public GLTF::Object {
  public:
    GLTF::Buffer* buffer;
    int byteOffset;
    int byteLength;
    GLTF::Constants::WebGL target;

    BufferView(unsigned char* data,
      int dataLength,
      GLTF::Constants::WebGL target
    );
  };
};
