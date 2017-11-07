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
			PBR_METALLIC_ROUGHNESS,
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
			GLTF::Texture* bumpTexture = NULL;

			void writeJSON(void* writer, GLTF::Options* options);
		};

		GLTF::Technique* technique = NULL;
		Type type = Type::UNKNOWN;
		Values* values = NULL;

		Material();
		GLTF::Material * getInstancedEffect(std::string name);
		bool hasTexture();
		virtual void writeJSON(void* writer, GLTF::Options* options);
	};

	class MaterialPBR : public GLTF::Material {
	public: 
		class Texture : public GLTF::Object {
		public:
			int scale = -1;
			GLTF::Texture* texture = NULL;
			int texCoord = -1;

			void writeJSON(void* writer, GLTF::Options* options);
		};

		class MetallicRoughness : public GLTF::Object {
		public:
			float* baseColorFactor = NULL;
			Texture* baseColorTexture = NULL;
			float metallicFactor = -1.0;
			float roughnessFactor = -1.0;
			Texture* metallicRoughnessTexture = NULL;

			void writeJSON(void* writer, GLTF::Options* options);
		};

		class SpecularGlossiness : public GLTF::Object {
		public:
			float* diffuseFactor = NULL;
			Texture* diffuseTexture = NULL;
			float* specularFactor = NULL;
			Texture* specularGlossinessTexture = NULL;
			float* glossinessFactor = NULL;

			void writeJSON(void* writer, GLTF::Options* options);
		};

		MetallicRoughness* metallicRoughness = NULL;
		Texture* normalTexture = NULL;
		Texture* occlusionTexture = NULL;
		float* emissiveFactor = NULL;
		Texture* emissiveTexture = NULL;
		SpecularGlossiness* specularGlossiness = NULL;

		MaterialPBR();
		void writeJSON(void* writer, GLTF::Options* options);
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
		GLTF::Material* getMaterial(std::vector<GLTF::MaterialCommon::Light*> lights, bool hasColorAttribute);
		std::string getTechniqueKey();
		GLTF::MaterialPBR* getMaterialPBR(bool specularGlossiness);
		virtual void writeJSON(void* writer, GLTF::Options* options);
	};
}
