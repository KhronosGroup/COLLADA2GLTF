#pragma once

#include <map>
#include <string>
#include <vector>

#include "GLTFAccessor.h"
#include "GLTFNode.h"
#include "GLTFObject.h"
#include "GLTFSampler.h"

namespace GLTF {
  class Animation : GLTF::Object {
  public:
    class Channel : GLTF::Object {
    public:
      class Target : GLTF::Object {
      public:
        GLTF::Node* target;
        enum Path {
          TRANSLATION,
          ROTATION,
          SCALE
        };
        Path path;
      };
    };

    std::vector<Channel*> channels;
    std::map<std::string, GLTF::Accessor*> parameters;
    std::vector<GLTF::Sampler*> samplers;
  };
}
