#pragma once

#include <vector>

#include "GLTFNode.h"
#include "GLTFObject.h"

namespace GLTF {
	class Scene : public GLTF::Object {
	public:
		std::vector<GLTF::Node*> nodes;

		virtual std::string typeName();
		virtual void writeJSON(void* writer, GLTF::Options* options);
	};
}
