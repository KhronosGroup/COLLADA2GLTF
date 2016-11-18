#pragma once

#include "Object.h"

namespace GLTF {
  class Buffer : public GLTF::Object {
  public:
    unsigned char* data;
    int byteLength;

    Buffer(unsigned char* data, int dataLength);
  };
};
