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

	  virtual std::string typeName();
	  virtual void writeJSON(void* writer, GLTF::Options* options);
  };

  class CameraOrthographic : public GLTF::Camera {
  public:
	  float xmag;
	  float ymag;

	  CameraOrthographic() {
		  type = Type::ORTHOGRAPHIC;
	  }
	  virtual void writeJSON(void* writer, GLTF::Options* options);
  };

  class CameraPerspective : public GLTF::Camera {
  public:
	  float aspectRatio = 0;
	  float yfov;

	  CameraPerspective() {
		  type = Type::PERSPECTIVE;
	  }
	  virtual void writeJSON(void* writer, GLTF::Options* options);
  };
}
