#pragma once

#include <string>

#include "GLTFObject.h"
#include "GLTFConstants.h"

namespace GLTF {
	class Shader : public GLTF::Object {
	public:
		std::string source;
		GLTF::Constants::WebGL type;
		std::string uri;

		virtual std::string typeName();
		virtual void writeJSON(void* writer, GLTF::Options* options);
	};
}
