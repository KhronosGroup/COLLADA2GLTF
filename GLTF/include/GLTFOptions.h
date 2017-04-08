#pragma once

#include <string>
#include <experimental/filesystem>

using namespace std::experimental::filesystem;

namespace GLTF {
	class Options {
	public:
		std::string name;
		bool embeddedBuffers = true;
		bool embeddedTextures = true;
		bool embeddedShaders = true;
		bool binary = false;
		bool materialsCommon = false;
		bool glsl = false;
		bool specularGlossiness = false;
		path metallicRoughnessTexturePath = "";
	};
}