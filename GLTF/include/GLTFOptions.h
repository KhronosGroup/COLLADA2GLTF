#pragma once

#include <string>

namespace GLTF {
	class Options {
	public:
		bool embeddedBuffers = true;
		bool embeddedTextures = true;
		bool binary = false;
	};
}