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
			void getTransformTRS(TransformTRS* out);
			TransformTRS* getTransformTRS();
		};

		class TransformTRS : public Transform {
		public:
			float translation[3];
			float rotation[4];
			float scale[3];

			TransformTRS();
			TransformMatrix* getTransformMatrix();
		};

		static int INSTANCE_COUNT;

		GLTF::Camera* camera;
		std::vector<GLTF::Node*> children;
		std::vector<GLTF::Node*> skeletons;
		GLTF::Skin* skin = NULL;
		std::string jointName;
		std::vector<GLTF::Mesh*> meshes;

		Transform* transform = NULL;

		Node();
		Node(std::string id);

		virtual void writeJSON(void* writer);
	};
}
