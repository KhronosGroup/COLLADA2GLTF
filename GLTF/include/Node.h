#pragma once

#include <set>
#include <string>
#include <vector>

#include "Camera.h"
#include "Mesh.h"
#include "Object.h"
#include "Skeleton.h"
#include "Skin.h"

namespace GLTF {
  class Node : GLTF::Object {
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

    GLTF::Camera* camera;
    std::vector<GLTF::Node*> children;
    std::vector<GLTF::Skeleton*> skeletons;
    GLTF::Skin* skin;
    std::string jointName;
    std::vector<GLTF::Mesh*> meshes;

    Transform* transform;
  };
}
