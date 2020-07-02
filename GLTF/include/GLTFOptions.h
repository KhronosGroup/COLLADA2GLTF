// Copyright 2020 The Khronos® Group Inc.
#pragma once

#include <string>
#include <vector>

namespace GLTF {
class Options {
 public:
  std::string name;
  bool embeddedBuffers = true;
  bool embeddedTextures = true;
  bool embeddedShaders = true;
  bool binary = false;
  bool lockOcclusionMetallicRoughness = false;
  bool materialsCommon = false;
  bool doubleSided = false;
  bool glsl = false;
  bool specularGlossiness = false;
  bool preserveUnusedSemantics = false;
  std::string version = "2.0";
  std::vector<std::string> metallicRoughnessTexturePaths;
  // For Draco compression extension.
  bool dracoCompression = false;
  int positionQuantizationBits = 14;
  int normalQuantizationBits = 10;
  int texcoordQuantizationBits = 10;
  int colorQuantizationBits = 8;
  int jointQuantizationBits = 8;
  bool writeAbsoluteUris = false;
};
}  // namespace GLTF
