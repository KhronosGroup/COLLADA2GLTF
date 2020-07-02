// Copyright 2020 The Khronos® Group Inc.
#pragma once

#include <string>

#include "GLTFConstants.h"
#include "GLTFObject.h"

namespace GLTF {
class Shader : public GLTF::Object {
 public:
  std::string source;
  GLTF::Constants::WebGL type;
  std::string uri;

  virtual std::string typeName();
  virtual void writeJSON(void* writer, GLTF::Options* options);
};
}  // namespace GLTF
