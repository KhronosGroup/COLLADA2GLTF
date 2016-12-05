#pragma once

#include <string>
#include <vector>

namespace GLTF {
  class Extension;
  class Object {
  public:
    std::string id;
    std::string name;
	std::vector<GLTF::Extension*> extensions;
	std::vector<GLTF::Object*> extras;

    void addExtension(GLTF::Extension* extension);
    void addExtra(GLTF::Object* extra);

	virtual GLTF::Object* clone();
    virtual void writeJSON(void* writer);
  };
}
