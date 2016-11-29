#pragma once

#include <map>
#include <string>

#include "GLTFAccessor.h"
#include "GLTFMaterial.h"
#include "GLTFObject.h"

namespace GLTF {
  class Primitive : public GLTF::Object {
  public:
    enum Mode {
      POINTS = 0,
      LINES = 1,
      LINE_LOOP = 2,
      LINE_STRIP = 3,
      TRIANGLES = 4,
      TRIANGLE_STRIP = 5,
      TRIANGLE_FAN = 6
    };
    std::map<std::string, GLTF::Accessor*>* attributes;
    GLTF::Accessor* indices;
    GLTF::Material* material;
    Mode mode;
  };
}
