#pragma once

#include <vector>

#include "GLTFObject.h"
#include "GLTFPrimitive.h"

namespace GLTF {
	class Mesh : public GLTF::Object {
	public:
		std::vector<GLTF::Primitive*> primitives;

		virtual GLTF::Object* clone();
		virtual void writeJSON(void* writer);
	};
}
