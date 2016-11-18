#pragma once

#include <map>
#include <string>

#include "Accessor.h"
#include "Material.h"
#include "Object.h"

namespace GLTF {
  class Primitive : GLTF::Object {
  public:
    std::map<std::string, GLTF::Accessor*> attributes;
    GLTF::Accessor* indices;
    GLTF::Material* material;
    int mode;
  };
}
