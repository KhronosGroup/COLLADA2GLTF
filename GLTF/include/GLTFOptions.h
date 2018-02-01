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
		bool glsl = false;
		bool specularGlossiness = false;
		std::vector<std::string> metallicRoughnessTexturePaths;
		// For Draco compression extension.
		bool dracoCompression = false;
		int positionQuantizationBits = 14;
		int normalQuantizationBits = 10;
		int texcoordQuantizationBits = 10;
		int colorQuantizationBits = 8;
		int jointQuantizationBits = 8;
		bool useUintIndices = false;
	};
}
