#pragma once

#include <set>
#include <string>
#include <vector>

#include "GLTFObject.h"
#include "GLTFScene.h"

namespace GLTF {
	class Asset : public GLTF::Object {
	public:
		class Profile : public GLTF::Object {
		public:
			std::string api;
			std::string version;
			virtual void writeJSON(void* writer);
		};

		class Metadata : public GLTF::Object {
		public:
			std::string copyright;
			std::string generator;
			bool premultipliedAlpha;
			Profile* profile = NULL;
			std::string version;
			virtual void writeJSON(void* writer);
		};

		Metadata* metadata = NULL;
		std::set<std::string> extensionsUsed;

		std::vector<GLTF::Scene*> scenes;
		int scene = -1;

		GLTF::Scene* getDefaultScene();
		virtual void writeJSON(void* writer);
		std::string toString();
	};
}
