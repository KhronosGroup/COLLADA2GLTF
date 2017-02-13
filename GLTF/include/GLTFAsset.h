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
		std::set<std::string> extensions;

		std::vector<GLTF::Scene*> scenes;
		std::vector<GLTF::Animation*> animations;
		int scene = -1;

		Asset();
		GLTF::Scene* getDefaultScene();
		std::set<GLTF::Node*> getAllNodes();
		std::set<GLTF::Mesh*> getAllMeshes();
		std::set<GLTF::Primitive*> getAllPrimitives();
		std::set<GLTF::Skin*> getAllSkins();
		std::set<GLTF::Material*> getAllMaterials();
		std::set<GLTF::Technique*> getAllTechniques();
		std::set<GLTF::Program*> getAllPrograms();
		std::set<GLTF::Shader*> getAllShaders();
		std::set<GLTF::Texture*> getAllTextures();
		std::set<GLTF::Image*> getAllImages();
		void removeUnusedSemantics();
		void separateSkeletonNodes();
		void removeUnusedNodes();
		GLTF::Buffer* packAccessors();
		virtual void writeJSON(void* writer, GLTF::Options* options);
	};
}
