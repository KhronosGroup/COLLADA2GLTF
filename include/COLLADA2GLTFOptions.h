// Copyright 2020 The Khronos® Group Inc.
#pragma once

#include <string>

#include "GLTFOptions.h"

namespace COLLADA2GLTF {
class Options : public GLTF::Options {
 public:
  std::string inputPath;
  std::string basePath;
  std::string outputPath;
  bool invertTransparency = false;
};
}  // namespace COLLADA2GLTF
