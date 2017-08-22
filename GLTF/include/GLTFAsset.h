#pragma once

#include <set>
#include <string>
#include <vector>

#ifdef USE_DRACO
#include <memory>
#include "draco/compression/encode.h"
#include "GLTFDracoExtension.h"
#endif

#include "GLTFAnimation.h"
#include "GLTFObject.h"
#include "GLTFScene.h"

namespace GLTF {
	class Asset : public GLTF::Object {
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
		void removeUnusedSemantics();
		void removeUnusedNodes(GLTF::Options* options);
		GLTF::Buffer* packAccessors();

#ifdef USE_DRACO
		bool compressPrimitives();
		GLTF::Buffer* packAccessorsWithCompressedAssets();
#endif

		void requireExtension(std::string extension);
		void useExtension(std::string extension);
		virtual void writeJSON(void* writer, GLTF::Options* options);
	};
}
