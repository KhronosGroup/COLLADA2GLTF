#pragma once

#include "GLTFObject.h"

namespace GLTF {
	class Buffer : public GLTF::Object {
	public:
		static int INSTANCE_COUNT;
		unsigned char* data = NULL;
		int byteLength;

		Buffer(unsigned char* data, int dataLength);
		
		virtual void writeJSON(void* writer);
	};
};
