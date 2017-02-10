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
			virtual void writeJSON(void* writer);
		};

		class Metadata : public GLTF::Object {
		public:
			std::string copyright;
			std::string generator = "COLLADA2GLTF";
			bool premultipliedAlpha = true;
			Profile* profile = NULL;
			std::string version = "2.0";
			virtual void writeJSON(void* writer);
		};

		Metadata* metadata = NULL;
		std::set<std::string> extensionsUsed;

		std::vector<GLTF::Scene*> scenes;
		std::vector<GLTF::Animation*> animations;
		int scene = -1;

		Asset();
		GLTF::Scene* getDefaultScene();
		void separateSkeletonNodes();
		void removeUnusedNodes();
		virtual void writeJSON(void* writer);
	};
}
