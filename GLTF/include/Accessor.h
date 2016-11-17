#pragma once

#include <string>

#include "Object.h"

namespace GLTF {
  class Accessor : GLTF::Object {
  public:
    /* https://github.com/KhronosGroup/glTF/tree/master/specification#reference-accessor */
    std::string bufferView;
    int byteOffset;
    int byteStride;
    int componentType;
    int count;
    std::string type;
    int max[];
    int min[];
  };
};
