#pragma once

#include "GLTFObject.h"

namespace GLTF {
	class Image : public GLTF::Object {
	public:
		std::string uri;

		virtual void writeJSON(void* writer);
	};
}