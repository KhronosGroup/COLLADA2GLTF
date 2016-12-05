#pragma once

#include <set>
#include <string>
#include <vector>

#include "GLTFCamera.h"
#include "GLTFMesh.h"
#include "GLTFObject.h"
#include "GLTFSkeleton.h"
#include "GLTFSkin.h"

namespace GLTF {
	class Node : public GLTF::Object {
	public:
		class Transform {
			virtual void concat(Transform* transform);
		};

		class TransformTRS;
		class TransformMatrix : Transform {
			int matrix[4][4];

			TransformTRS* getTransformTRS();
		};

		class TransformTRS : Transform {
			int translation[3];
			int rotation[3];
			int scale[3];

			TransformMatrix* getTransformMatrix();
		};

		static int INSTANCE_COUNT;

		GLTF::Camera* camera;
		std::vector<GLTF::Node*> children;
		std::vector<GLTF::Skeleton*> skeletons;
		GLTF::Skin* skin;
		std::string jointName;
		std::vector<GLTF::Mesh*> meshes;

		Transform* transform;

		Node();
		Node(std::string id);

		virtual void writeJSON(void* writer);
	};
}
