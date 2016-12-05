#pragma once

#include <vector>

#include "GLTFNode.h"
#include "GLTFObject.h"

namespace GLTF {
	class Scene : public GLTF::Object {
	public:
		static int INSTANCE_COUNT;
		std::vector<GLTF::Node*> nodes;

		Scene();
		Scene(std::string id);

		virtual void writeJSON(void* writer);
	};
}
