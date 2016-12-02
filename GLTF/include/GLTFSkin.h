#pragma once

#include <vector>

#include "GLTFAccessor.h"
#include "GLTFNode.h"
#include "GLTFObject.h"

namespace GLTF {
	class Node;
	class Skin : GLTF::Object {
	public:
		float bindShapeMatrix[16] = {
			1.0, 0.0, 0.0, 0.0,
			0.0, 1.0, 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0,
			0.0, 0.0, 0.0, 1.0
		};
		GLTF::Accessor* inverseBindMatrices = NULL;
		std::vector<GLTF::Node*> joints;
	};
}
