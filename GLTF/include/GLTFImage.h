#pragma once

#include "GLTFBufferView.h"
#include "GLTFObject.h"

namespace GLTF {
	class Image : public GLTF::Object {
	public:
		std::string uri;
		unsigned char* data;
		size_t byteLength;
		std::string mimeType;
		GLTF::BufferView* bufferView = NULL;

		Image(std::string uri, unsigned char* data, size_t byteLength, std::string fileExtension);
		std::pair<int, int> getDimensions();
		virtual void writeJSON(void* writer, GLTF::Options* options);
	};
}