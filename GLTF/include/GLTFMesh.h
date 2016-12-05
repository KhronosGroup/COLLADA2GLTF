#pragma once

#include <vector>

#include "GLTFObject.h"
#include "GLTFPrimitive.h"

namespace GLTF {
	class Mesh : public GLTF::Object {
	public:
		static int INSTANCE_COUNT;
		std::vector<GLTF::Primitive*> primitives;

		GLTF::Mesh();
		GLTF::Mesh(std::string id);
		virtual GLTF::Object* clone();
		virtual void writeJSON(void* writer);
	};
}
