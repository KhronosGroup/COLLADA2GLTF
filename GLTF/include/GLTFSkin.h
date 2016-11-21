#pragma once

#include <vector>

#include "GLTFAccessor.h"
#include "GLTFNode.h"
#include "GLTFObject.h"

namespace GLTF {
  class Node;
  class Skin : GLTF::Object {
  public:
    int bindShapeMatrix[16];
    GLTF::Accessor* inverseBindMatrices;
    std::vector<GLTF::Node*> joints;
  };
}
