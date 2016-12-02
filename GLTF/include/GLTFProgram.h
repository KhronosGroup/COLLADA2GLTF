#pragma once

#include <string>
#include <vector>

#include "GLTFObject.h"
#include "GLTFShader.h"

namespace GLTF {
  class Program : public GLTF::Object {
  public:
    std::vector<std::string> attributes;
	GLTF::Shader* fragmentShader = NULL;
	GLTF::Shader* vertexShader = NULL;
  };
}
