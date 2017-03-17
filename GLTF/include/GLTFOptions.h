#pragma once

#include <string>

namespace GLTF {
	class Options {
	public:
		std::string name;
		bool embeddedBuffers = true;
		bool embeddedTextures = true;
		bool embeddedShaders = true;
		bool binary = false;
		bool materialsCommon = false;
                bool dracoCompression = false;
	};
}
