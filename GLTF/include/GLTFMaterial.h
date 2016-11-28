#pragma once

#include <string>

#include "GLTFObject.h"
#include "GLTFTechnique.h"
#include "GLTFTexture.h"

namespace GLTF {
  class Material : public GLTF::Object {
  public:
    class Values {
    public:
      float ambient[4];
      float diffuse[4];
      GLTF::Texture* diffuseTexture;
      float emission[4];
      float shininess[1];
      float specular[4];
    };

    GLTF::Technique* technique;
    Values* values;
  };
}
