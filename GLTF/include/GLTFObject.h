#pragma once

#include "GLTFOptions.h"

#include <map>
#include <string>
#include <vector>

namespace GLTF {
  class Extension;
  class Object {
  public:
	  int id = -1;
    std::string name;
	std::map<std::string, GLTF::Extension*> extensions;
	std::map<std::string, GLTF::Object*> extras;

	virtual GLTF::Object* clone();
    virtual void writeJSON(void* writer, GLTF::Options* options);
  };
}
