#pragma once

#include <vector>

#include "Accessor.h"
#include "Node.h"
#include "Object.h"

namespace GLTF {
  class Node;
  class Skin : GLTF::Object {
  public:
    int bindShapeMatrix[16];
    GLTF::Accessor* inverseBindMatrices;
    std::vector<GLTF::Node*> joints;
  };
}
