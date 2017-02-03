#pragma once

#include <string>

namespace GLTF {
	namespace Base64 {
		char* encode(unsigned char* data, size_t length);
		std::string decode(std::string uri);
	}
}