#pragma once

#include "GLTFObject.h"

namespace GLTF {
	class Image : public GLTF::Object {
	public:
		std::string uri;

		Image(std::string uri);
		Image(unsigned char* data, size_t length, std::string mimeType);
		virtual void writeJSON(void* writer);
	};
}