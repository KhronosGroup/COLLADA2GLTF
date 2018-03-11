#pragma once

#include <set>
#include <string>
#include <vector>

#include "GLTFAnimation.h"
#include "GLTFDracoExtension.h"
#include "GLTFObject.h"
#include "GLTFScene.h"

#include "draco/compression/encode.h"

namespace GLTF {
	class Asset : public GLTF::Object {
	private:
		std::vector<GLTF::MaterialCommon::Light*> _ambientLights;
	public:
		class Metadata : public GLTF::Object {
		public:
			std::string copyright;
			std::string generator = "COLLADA2GLTF";
			std::string version = "2.0";
			virtual void writeJSON(void* writer, GLTF::Options* options);
		};

		GLTF::Sampler* globalSampler = NULL;

		Metadata* metadata = NULL;
		std::set<std::string> extensionsUsed;
		std::set<std::string> extensionsRequired;

		std::vector<GLTF::Scene*> scenes;
		std::vector<GLTF::Animation*> animations;
		int scene = -1;

		Asset();
		GLTF::Scene* getDefaultScene();
		std::vector<GLTF::Accessor*> getAllAccessors();
		std::vector<GLTF::Node*> getAllNodes();
		std::vector<GLTF::Mesh*> getAllMeshes();
		std::vector<GLTF::Primitive*> getAllPrimitives();
		std::vector<GLTF::Skin*> getAllSkins();
		std::vector<GLTF::Material*> getAllMaterials();
		std::vector<GLTF::Technique*> getAllTechniques();
		std::vector<GLTF::Program*> getAllPrograms();
		std::vector<GLTF::Shader*> getAllShaders();
		std::vector<GLTF::Texture*> getAllTextures();
		std::vector<GLTF::Image*> getAllImages();
		std::vector<GLTF::Accessor*> getAllPrimitiveAccessors(GLTF::Primitive* primitive) const;
		void mergeAnimations();
		void removeUnusedSemantics();
		void removeUnusedNodes(GLTF::Options* options);
		GLTF::Buffer* packAccessors();

		// Functions for Draco compression extension.
		std::vector<GLTF::BufferView*> getAllCompressedBufferView();
		bool compressPrimitives(GLTF::Options* options);
		void removeUncompressedBufferViews();
		void removeAttributeFromDracoExtension(GLTF::Primitive* primitive, const std::string &semantic);

		void requireExtension(std::string extension);
		void useExtension(std::string extension);
		virtual void writeJSON(void* writer, GLTF::Options* options);
	};
}
