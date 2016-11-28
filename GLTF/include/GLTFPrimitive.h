#pragma once

#include <map>
#include <string>

#include "GLTFAccessor.h"
#include "GLTFMaterial.h"
#include "GLTFObject.h"

namespace GLTF {
  class Primitive : public GLTF::Object {
  public:
    std::map<std::string, GLTF::Accessor*>* attributes;
    GLTF::Accessor* indices;
    GLTF::Material* material;
    int mode;
  };
}
