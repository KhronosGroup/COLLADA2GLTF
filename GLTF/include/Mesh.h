#pragma once;

#include <vector>

#include "Object.h"

namespace GLTF {
  class Mesh : GLTF::Object {
  public:
    /* https://github.com/KhronosGroup/glTF/tree/master/specification#mesh */
    std::vector<GLTF::Primitive> primitives;
  };
}
