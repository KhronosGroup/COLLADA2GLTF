#pragma once

#include "GLTFConstants.h"
#include "GLTFObject.h"

namespace GLTF {
  class Sampler : public GLTF::Object {
  public:
	  static int INSTANCE_COUNT;

	  GLTF::Constants::WebGL magFilter = GLTF::Constants::WebGL::LINEAR;
	  GLTF::Constants::WebGL minFilter = GLTF::Constants::WebGL::NEAREST_MIPMAP_LINEAR;
	  GLTF::Constants::WebGL wrapS = GLTF::Constants::WebGL::REPEAT;
	  GLTF::Constants::WebGL wrapT = GLTF::Constants::WebGL::REPEAT;

	  Sampler();
	  virtual void writeJSON(void* writer);
  };
}
