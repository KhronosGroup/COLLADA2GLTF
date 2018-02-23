#pragma once

#include <set>
#include <string>
#include <vector>

#include "GLTFObject.h"
#include "GLTFShader.h"

namespace GLTF {
	class Program : public GLTF::Object {
	public:
		std::set<std::string> attributes;
		GLTF::Shader* fragmentShader = NULL;
		GLTF::Shader* vertexShader = NULL;

		virtual std::string typeName();
		virtual void writeJSON(void* writer, GLTF::Options* options);
	};
}
