#pragma once

#include <map>
#include <string>

#include "GLTFAccessor.h"
#include "GLTFDracoExtension.h"
#include "GLTFMaterial.h"
#include "GLTFObject.h"

namespace GLTF {
	class Primitive : public GLTF::Object {
	public:
		enum Mode {
			UNKNOWN = -1,
			POINTS = 0,
			LINES = 1,
			LINE_LOOP = 2,
			LINE_STRIP = 3,
			TRIANGLES = 4,
			TRIANGLE_STRIP = 5,
			TRIANGLE_FAN = 6,
		};

		class Target {
		public:
			std::map<std::string, GLTF::Accessor*> attributes;

			Target* clone(GLTF::Object* clone);
			void writeJSON(void* writer, GLTF::Options* options);
		};

		std::map<std::string, GLTF::Accessor*> attributes;
		GLTF::Accessor* indices = NULL;
		GLTF::Material* material = NULL;
		Mode mode = Mode::UNKNOWN;
		std::vector<Target*> targets;

        ~Primitive();

		virtual GLTF::Object* clone(GLTF::Object* clone);
		virtual void writeJSON(void* writer, GLTF::Options* options);
	};
}
