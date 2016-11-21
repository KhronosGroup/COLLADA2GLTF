#pragma once

#include <set>
#include <string>
#include <vector>

#include "GLTFObject.h"
#include "GLTFScene.h"

namespace GLTF {
  class Asset : GLTF::Object  {
  public:
    class Profile : GLTF::Object {
    public:
      std::string api;
      std::string version;
    };

    class Metadata : GLTF::Object {
    public:
      std::string copyright;
      std::string generator;
      bool premultipliedAlpha;
      Profile* profile;
      std::string version;
    };

    Metadata* metadata;
    std::set<std::string> extensionsUsed;

    std::vector<GLTF::Scene*> scenes;
    int scene;
  };
}
