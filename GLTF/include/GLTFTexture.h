// Copyright 2020 The Khronos® Group Inc.
#pragma once

#include <string>

#include "GLTFConstants.h"
#include "GLTFImage.h"
#include "GLTFObject.h"
#include "GLTFSampler.h"

namespace GLTF {
class Texture : public GLTF::Object {
 public:
  GLTF::Sampler* sampler = NULL;
  GLTF::Image* source = NULL;

  virtual std::string typeName();
  virtual void writeJSON(void* writer, GLTF::Options* options);
};
}  // namespace GLTF
