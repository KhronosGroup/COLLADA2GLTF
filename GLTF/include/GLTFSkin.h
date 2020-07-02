// Copyright 2020 The Khronos® Group Inc.
#pragma once

#include <string>
#include <vector>

#include "GLTFAccessor.h"
#include "GLTFObject.h"

namespace GLTF {
class Node;

class Skin : public GLTF::Object {
 public:
  Accessor* inverseBindMatrices = NULL;
  Node* skeleton = NULL;
  std::vector<Node*> joints;

  virtual std::string typeName();
  virtual void writeJSON(void* writer, GLTF::Options* options);
};
}  // namespace GLTF
