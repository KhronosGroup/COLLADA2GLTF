#pragma

#include "GLTFObject.h"
#include "GLTFProgram.h"

namespace GLTF {
	class Technique : public GLTF::Object {
	public:
		GLTF::Program* program = NULL;
	};
}
