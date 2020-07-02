// Copyright 2020 The Khronos® Group Inc.
#pragma once

#include <map>
#include <string>
#include <vector>

#include "GLTFOptions.h"

namespace GLTF {
class Extension;
class Object {
 public:
  virtual ~Object();

  int id = -1;
  std::string stringId;
  std::string name;
  std::map<std::string, GLTF::Extension*> extensions;
  std::map<std::string, GLTF::Object*> extras;

  std::string getStringId();
  virtual std::string typeName();
  virtual GLTF::Object* clone(GLTF::Object* clone);
  virtual void writeJSON(void* writer, GLTF::Options* options);
};
}  // namespace GLTF
