#pragma once

#include <string>
#include <vector>
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
		bool lockOcclusionMetallicRoughness = false;
		bool materialsCommon = false;
                bool dracoCompression = true;
		bool glsl = false;
		bool specularGlossiness = false;
		std::vector<std::string> metallicRoughnessTexturePaths;
	};
}
