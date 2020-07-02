// Copyright 2020 The Khronos® Group Inc.
#pragma once

#include <string>
#include <vector>

#include "GLTFNode.h"
#include "GLTFObject.h"

namespace GLTF {
class Scene : public GLTF::Object {
 public:
  std::vector<GLTF::Node*> nodes;

  virtual std::string typeName();
  virtual void writeJSON(void* writer, GLTF::Options* options);
};
}  // namespace GLTF
