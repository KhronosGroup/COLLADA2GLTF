#pragma once

#ifdef USE_DRACO

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
		DracoExtension() : draco_mesh(new draco::Mesh()) {} 
		GLTF::BufferView* bufferView = NULL;
		std::unordered_map<std::string, int> attribute_to_id;
		
		std::unique_ptr<draco::Mesh> draco_mesh;
		virtual void writeJSON(void* writer, GLTF::Options* options);
	};
}

#endif // USE_DRACO
