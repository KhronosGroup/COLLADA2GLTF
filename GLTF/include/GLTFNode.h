#pragma once

#include <set>
#include <string>
#include <vector>

#include "GLTFCamera.h"
#include "GLTFMesh.h"
#include "GLTFObject.h"
#include "GLTFSkin.h"

namespace GLTF {
	class Node : public GLTF::Object {
	public:
		class Transform {
		public:
			enum Type {
				TRS,
				MATRIX
			};

			Type type;
		};

		class TransformTRS;
		class TransformMatrix : public Transform {
		public:
			float matrix[16];

			TransformMatrix();
			TransformMatrix(float a00, float a01, float a02, float a03, 
				float a10, float a11, float a12, float a13, 
				float a20, float a21, float a22, float a23, 
				float a30, float a31, float a32, float a33);

			void premultiply(TransformMatrix* transform);
			void premultiply(TransformMatrix* transform, TransformMatrix* destination);
			void scaleUniform(float scale);
			bool isIdentity();
			void getTransformTRS(TransformTRS* out);
			TransformTRS* getTransformTRS();
		};

		class TransformTRS : public Transform {
		public:
			float translation[3];
			float rotation[4];
			float scale[3];

			TransformTRS();
			bool isIdentityTranslation();
			bool isIdentityRotation();
			bool isIdentityScale();
			TransformMatrix* getTransformMatrix();
		};

		GLTF::Camera* camera = NULL;
		std::vector<GLTF::Node*> children;
		GLTF::Skin* skin = NULL;
		std::string jointName;
		GLTF::Mesh* mesh = NULL;
		GLTF::MaterialCommon::Light* light = NULL;

		Transform* transform = NULL;

		virtual std::string typeName();
		virtual GLTF::Object* clone(GLTF::Object* clone);
		virtual void writeJSON(void* writer, GLTF::Options* options);
	};
}
