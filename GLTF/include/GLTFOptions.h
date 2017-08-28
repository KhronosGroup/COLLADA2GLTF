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
		int position_quantization_bits = 14;
		int normal_quantization_bits = 10;
		int texcoord_quantization_bits = 10;
		int color_quantization_bits = 8;
		int joint_quantization_bits = 8;
	};
}