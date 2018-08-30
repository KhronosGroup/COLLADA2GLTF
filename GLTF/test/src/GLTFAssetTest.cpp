#include "GLTFAsset.h"
#include "GLTFAssetTest.h"

TEST(GLTFAssetTest, RemoveUnusedSemantics) {
  GLTF::Asset* asset = new GLTF::Asset();

  GLTF::Scene* scene = new GLTF::Scene();
  asset->scenes.push_back(scene);
  asset->scene = 0;

  GLTF::Node* node = new GLTF::Node();
  scene->nodes.push_back(node);

  GLTF::Mesh* mesh = new GLTF::Mesh();
  node->mesh = mesh;
  
  GLTF::Primitive* primitive = new GLTF::Primitive();
  mesh->primitives.push_back(primitive);

  GLTF::Material* material = new GLTF::Material();
  primitive->material = material;

  // Add an unused texture coordinate attribute
  primitive->attributes["TEXCOORD_0"] = NULL;

  EXPECT_EQ(primitive->attributes.size(), 1);
  asset->removeUnusedSemantics();
  EXPECT_EQ(primitive->attributes.size(), 0);

  // Add an unused and a used texture coordinaate
  primitive->attributes["TEXCOORD_0"] = NULL;
  primitive->attributes["TEXCOORD_1"] = (GLTF::Accessor*)1;

  material->values->ambientTexture = new GLTF::Texture();
  material->values->ambientTexCoord = 1;

  EXPECT_EQ(primitive->attributes.size(), 2);
  asset->removeUnusedSemantics();
  EXPECT_EQ(primitive->attributes.size(), 1);
  EXPECT_EQ(primitive->attributes["TEXCOORD_0"], (GLTF::Accessor*)1);
  EXPECT_EQ(material->values->ambientTexCoord, 0);
}