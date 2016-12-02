#pragma once

#include "GLTFBuffer.h"
#include "GLTFConstants.h"
#include "GLTFObject.h"

namespace GLTF {
	class BufferView : public GLTF::Object {
	public:
		GLTF::Buffer* buffer = NULL;
		int byteOffset = 0;
		int byteLength = 0;
		GLTF::Constants::WebGL target;

		BufferView(unsigned char* data,
			int dataLength,
			GLTF::Constants::WebGL target
		);
	};
};
