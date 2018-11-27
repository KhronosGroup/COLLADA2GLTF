#pragma once

#include "GLTFOptions.h"

#include <string>

namespace COLLADA2GLTF {
	class Options : public GLTF::Options {
	public:
		std::string inputPath;
		std::string basePath;
		std::string outputPath;
		bool invertTransparency = false;
	};
}