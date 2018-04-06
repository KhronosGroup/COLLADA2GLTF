#pragma once

#include <map>
#include <string>
#include <vector>

#include "GLTFAccessor.h"
#include "GLTFNode.h"
#include "GLTFObject.h"

namespace GLTF {
  class Animation : public GLTF::Object {
  public:
	enum class Path {
		TRANSLATION,
		ROTATION,
		SCALE,
		WEIGHTS
	};

	class Sampler : public GLTF::Object {
	public:
		GLTF::Accessor* input;
		std::string interpolation = "LINEAR";
		GLTF::Accessor* output;
		std::string inputString;
		std::string outputString;
		Path path;

		virtual std::string typeName();
		virtual void writeJSON(void* writer, GLTF::Options* options);
	};

    class Channel : public GLTF::Object {
    public:
		class Target : public GLTF::Object {
		public:
			GLTF::Node* node;
			Path path;

			virtual void writeJSON(void* writer, GLTF::Options* options);
		};

		GLTF::Animation::Sampler* sampler;
		Target* target;

		virtual void writeJSON(void* writer, GLTF::Options* options);
    };

    std::vector<Channel*> channels;

	virtual std::string typeName();
	virtual void writeJSON(void* writer, GLTF::Options* options);
  };
}
