#pragma once

#include <string>

#include "GLTFObject.h"
#include "GLTFTechnique.h"
#include "GLTFTexture.h"

namespace GLTF {
	class Material : public GLTF::Object {
	public:
		enum Type {
			TECHNIQUE,
			COMMON,
			UNKNOWN
		};

		class Values {
		public:
			float ambient[4] = { 0.0, 0.0, 0.0, 1.0 };
			float diffuse[4] = { 0.0, 0.0, 0.0, 1.0 };
			GLTF::Texture* diffuseTexture = NULL;
			float emission[4] = { 0.0, 0.0, 0.0, 1.0 };
			float shininess[1] = { 0.0 };
			float specular[4] = { 0.0, 0.0, 0.0, 1.0 };
		};

		Type type = Type::UNKNOWN;
		Values* values = NULL;
	};

	class MaterialTechnique : public GLTF::Material {
	public:
		GLTF::Technique* technique = NULL;
	};

	class MaterialCommon : public GLTF::Material {
	public:
		enum Shader {
			BLINN,
			PHONG,
			LAMBERT,
			CONSTANT
		};

		class ValuesCommon : Values {
		public:
			bool doubleSided = false;
			int jointCount = 0;
			bool transparent = false;
		};

		Shader shader;
	};
}
