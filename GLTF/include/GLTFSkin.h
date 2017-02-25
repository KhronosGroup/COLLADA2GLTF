#pragma once

#include <vector>

#include "GLTFAccessor.h"
#include "GLTFObject.h"

namespace GLTF {
	class Node;

	class Skin : public GLTF::Object {
	public:
		float* bindShapeMatrix = NULL;
		Accessor* inverseBindMatrices = NULL;
		std::vector<Node*> joints;

		Skin();
		virtual void writeJSON(void* writer, GLTF::Options* options);
	};
}
