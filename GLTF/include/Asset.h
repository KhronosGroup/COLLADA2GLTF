#pragma once

#include <string>
#include <vector>

#include "Object.h"
#include "Scene.h"

namespace GLTF {
  class Asset : GLTF::Object  {
  public:
    class Profile : GLTF::Object {
    public:
      std::string api;
      std::string version;
    }

    class Metadata : GLTF::Object {
    public:
      std::string copyright;
      std::string generator;
      bool premultipliedAlpha;
      Profile* profile;
      std::string version;
    }

    Metadata* metadata;
    set<std::string> extensionsUsed;

    vector<GLTF::Scene*> scenes;
    int scene;
  }
}
