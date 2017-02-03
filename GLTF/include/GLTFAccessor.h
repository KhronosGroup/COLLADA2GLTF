#pragma once

#include <string>

#include "GLTFBufferView.h"
#include "GLTFConstants.h"
#include "GLTFObject.h"

namespace GLTF {
	class Accessor : public GLTF::Object {
	public:
		static int INSTANCE_COUNT;
		enum class Type {
			SCALAR,
			VEC2,
			VEC3,
			VEC4,
			MAT2,
			MAT3,
			MAT4,
			UNKNOWN
		};

		GLTF::BufferView* bufferView = NULL;
		int byteOffset = 0;
		int byteStride = 0;
		GLTF::Constants::WebGL componentType;
		int count = 0;
		double* max = NULL;
		double* min = NULL;
		Type type = Type::UNKNOWN;

		Accessor(GLTF::Accessor::Type type, GLTF::Constants::WebGL componentType);

		Accessor(GLTF::Accessor::Type type,
			GLTF::Constants::WebGL componentType,
			unsigned char* data,
			int count,
			GLTF::Constants::WebGL target
		);

		Accessor(GLTF::Accessor::Type type,
			GLTF::Constants::WebGL componentType,
			unsigned char* data,
			int count,
			GLTF::BufferView* bufferView
		);

		static int getComponentByteLength(GLTF::Constants::WebGL componentType);
		static int getNumberOfComponents(GLTF::Accessor::Type type);

		virtual void writeJSON(void* writer);

		bool computeMinMax();
		int getByteStride();
		bool getComponentAtIndex(int index, double* component);
		int getComponentByteLength();
		int getNumberOfComponents();
		bool equals(GLTF::Accessor* accessor);
		const char* getTypeName();
	};
};
