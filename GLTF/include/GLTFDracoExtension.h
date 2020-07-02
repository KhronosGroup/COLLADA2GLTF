// Copyright 2020 The Khronos® Group Inc.
#pragma once

#include <map>
#include <memory>
#include <string>
#include <unordered_map>

#include "GLTFAccessor.h"
#include "GLTFBufferView.h"
#include "GLTFConstants.h"
#include "GLTFObject.h"
#include "draco/compression/encode.h"

namespace GLTF {
class DracoExtension : public GLTF::Object {
 public:
  DracoExtension() : dracoMesh(new draco::Mesh()) {}
  GLTF::BufferView* bufferView = NULL;
  std::unordered_map<std::string, int> attributeToId;

  std::unique_ptr<draco::Mesh> dracoMesh;
  virtual void writeJSON(void* writer, GLTF::Options* options);
};
}  // namespace GLTF
