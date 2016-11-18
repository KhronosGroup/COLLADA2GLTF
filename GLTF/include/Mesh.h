#pragma once

#include <vector>

#include "Object.h"
#include "Primitive.h"

namespace GLTF {
  class Mesh : public GLTF::Object {
  public:
    std::vector<GLTF::Primitive> primitives;
  };
}
