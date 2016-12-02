#pragma once

#include "GLTFObject.h"

namespace GLTF {
	class Buffer : public GLTF::Object {
	public:
		unsigned char* data = NULL;
		int byteLength;

		Buffer(unsigned char* data, int dataLength);
	};
};
