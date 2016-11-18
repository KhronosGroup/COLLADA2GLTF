#pragma once

#include <vector>

#include "Node.h"
#include "Object.h"

namespace GLTF {
  class Scene : GLTF::Object {
  public:
    std::vector<GLTF::Node*> nodes;
  };
}
