#pragma once

#include "GLTFConstants.h"
#include "GLTFImage.h"
#include "GLTFObject.h"
#include "GLTFSampler.h"

namespace GLTF {
	class Texture : public GLTF::Object {
	public:
		GLTF::Constants::WebGL format = GLTF::Constants::WebGL::RGBA;
		GLTF::Constants::WebGL internalFormat = GLTF::Constants::WebGL::RGBA;
		GLTF::Sampler* sampler = NULL;
		GLTF::Image* source = NULL;
		GLTF::Constants::WebGL target = GLTF::Constants::WebGL::TEXTURE_2D;
		GLTF::Constants::WebGL type = GLTF::Constants::WebGL::UNSIGNED_BYTE;
		
		virtual void writeJSON(void* writer);
	};
}
