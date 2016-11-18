#pragma once

#include <map>
#include <string>
#include <vector>

#include "Extension.h"

namespace GLTF {
  class Object {
  public:
    std::string id;
    std::string name;
    std::map<std::string, GLTF::Extension*> extensions;
    std::map<std::string, void*> extras;
  };
}
