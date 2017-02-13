#pragma once

#include <string>

#include "GLTFObject.h"
#include "GLTFTechnique.h"
#include "GLTFTexture.h"

namespace GLTF {
	class Material : public GLTF::Object {
	public:
		enum Type {
			MATERIAL,
			MATERIAL_COMMON,
			UNKNOWN
		};

		class Values {
		public:
			float* ambient = NULL;
			GLTF::Texture* ambientTexture = NULL;
			float* diffuse = NULL;
			GLTF::Texture* diffuseTexture = NULL;
			float* emission = NULL;
			GLTF::Texture* emissionTexture = NULL;
			float* specular = NULL;
			GLTF::Texture* specularTexture = NULL;
			float* shininess = NULL;
			float* transparency = NULL;

			void writeJSON(void* writer, GLTF::Options* options);
		};

		GLTF::Technique* technique = NULL;
		Type type = Type::UNKNOWN;
		Values* values = NULL;

		Material();
		bool hasTexture();
		virtual void writeJSON(void* writer, GLTF::Options* options);
	};


	class MaterialCommon : public GLTF::Material {
	public:
		enum Technique {
			BLINN,
			PHONG,
			LAMBERT,
			CONSTANT,
			UNKNOWN
		};

		class Light: public GLTF::Object {
		public:
			enum Type {
				AMBIENT,
				DIRECTIONAL,
				POINT,
				SPOT,
				UNKOWN
			};

			Type type = Type::UNKOWN;
			float color[4];
			float constantAttenuation;
			float linearAttenuation;
			float quadraticAttenuation;
			void* node = NULL;

			virtual void writeJSON(void* writer, GLTF::Options* options);
		};

		bool doubleSided = false;
		int jointCount = 0;
		bool transparent = false;

		MaterialCommon::Technique technique = MaterialCommon::Technique::UNKNOWN;

		MaterialCommon();
		const char* getTechniqueName();
		GLTF::Material* getMaterial(std::vector<GLTF::MaterialCommon::Light*> lights);
		std::string getTechniqueKey();
		virtual void writeJSON(void* writer, GLTF::Options* options);
	};
}
