#pragma once

#include <string>

namespace COLLADA2GLTF {
	class Options {
	public:
		std::string inputPath;
		std::string basePath;
		std::string outputPath;
		bool embedded = true;
	};
}