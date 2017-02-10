#pragma once

#include <set>
#include <string>
#include <vector>

#include "GLTFAnimation.h"
#include "GLTFObject.h"
#include "GLTFScene.h"

namespace GLTF {
	class Asset : public GLTF::Object {
	public:
		class Profile : public GLTF::Object {
		public:
			std::string api = "WebGL";
			std::string version = "1.0";
			virtual void writeJSON(void* writer, GLTF::Options* options);
		};

		class Metadata : public GLTF::Object {
		public:
			std::string copyright;
			std::string generator = "COLLADA2GLTF";
			bool premultipliedAlpha = true;
			Profile* profile = NULL;
			std::string version = "2.0";
			virtual void writeJSON(void* writer, GLTF::Options* options);
		};

		Metadata* metadata = NULL;
		std::set<std::string> extensionsUsed;

		std::vector<GLTF::Scene*> scenes;
		std::vector<GLTF::Animation*> animations;
		int scene = -1;

		Asset();
		GLTF::Scene* getDefaultScene();
		std::vector<GLTF::Node*> getAllNodes();
		std::vector<GLTF::Mesh*> getAllMeshes();
		std::vector<GLTF::Primitive*> getAllPrimitives();
		std::vector<GLTF::Skin*> getAllSkins();
		std::vector<GLTF::Material*> getAllMaterials();
		std::vector<GLTF::Texture*> getAllTextures();
		std::vector<GLTF::Image*> getAllImages();
		void removeUnusedSemantics();
		void separateSkeletonNodes();
		void removeUnusedNodes();
		GLTF::Buffer* packAccessors();
		virtual void writeJSON(void* writer, GLTF::Options* options);
	};
}
