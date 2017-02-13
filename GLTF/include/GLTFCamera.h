#pragma once

#include "GLTFObject.h"

namespace GLTF {
  class Camera : public GLTF::Object {
  public:
	  enum Type {
		  PERSPECTIVE,
		  ORTHOGRAPHIC,
		  UNKNOWN
	  };
	  Type type = Type::UNKNOWN;
	  float zfar;
	  float znear;

	  virtual void writeJSON(void* writer, GLTF::Options* options);
  };

  class CameraOrthographic : public GLTF::Camera {
  public:
	  float xmag;
	  float ymag;

	  virtual void writeJSON(void* writer, GLTF::Options* options);
  };

  class CameraPerspective : public GLTF::Camera {
  public:
	  float aspectRatio;
	  float yfov;

	  virtual void writeJSON(void* writer, GLTF::Options* options);
  };
}
