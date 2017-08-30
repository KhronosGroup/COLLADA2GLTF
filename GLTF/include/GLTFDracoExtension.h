#pragma once

#include <map>
#include <string>
#include <unordered_map>

#include "draco/compression/encode.h"
#include "GLTFAccessor.h"
#include "GLTFBufferView.h"
#include "GLTFConstants.h"
#include "GLTFObject.h"

namespace GLTF {
	class DracoExtension : public GLTF::Object {
	public:
		DracoExtension() : dracoMesh(new draco::Mesh()) {} 
		GLTF::BufferView* bufferView = NULL;
		std::unordered_map<std::string, int> attributeToId;
		
		std::unique_ptr<draco::Mesh> dracoMesh;
		virtual void writeJSON(void* writer, GLTF::Options* options);
	};
}
