// Copyright 2020 The Khronos® Group Inc.
#include "COLLADA2GLTFWriterTest.h"

#include <vector>

#include "COLLADABU.h"
#include "COLLADAFW.h"

COLLADA2GLTFWriterTest::COLLADA2GLTFWriterTest() {
  asset = new GLTF::Asset();
  options = new COLLADA2GLTF::Options();
  COLLADASaxFWL::Loader* loader = new COLLADASaxFWL::Loader();
  extrasHandler = new COLLADA2GLTF::ExtrasHandler(loader);
  writer = new COLLADA2GLTF::Writer(loader, asset, options, extrasHandler);
}

COLLADA2GLTFWriterTest::~COLLADA2GLTFWriterTest() {
  delete writer;
  delete asset;
}

TEST_F(COLLADA2GLTFWriterTest, WriteVisualScene_SingleNode) {
  COLLADAFW::VisualScene* visualScene = new COLLADAFW::VisualScene(
      COLLADAFW::UniqueId(COLLADAFW::COLLADA_TYPE::VISUAL_SCENE, 0, 0));
  COLLADAFW::Node* node = new COLLADAFW::Node(
      COLLADAFW::UniqueId(COLLADAFW::COLLADA_TYPE::NODE, 0, 0));
  visualScene->getRootNodes().append(node);
  this->writer->writeVisualScene(visualScene);
  GLTF::Scene* scene = this->asset->getDefaultScene();
  ASSERT_TRUE(scene != NULL);
  std::vector<GLTF::Node*> sceneNodes = scene->nodes;
  EXPECT_EQ(sceneNodes.size(), 1);
}

TEST_F(COLLADA2GLTFWriterTest, WriteVisualScene_MultipleNodes) {
  COLLADAFW::VisualScene* visualScene = new COLLADAFW::VisualScene(
      COLLADAFW::UniqueId(COLLADAFW::COLLADA_TYPE::VISUAL_SCENE, 0, 0));
  COLLADAFW::Node* nodeOne = new COLLADAFW::Node(
      COLLADAFW::UniqueId(COLLADAFW::COLLADA_TYPE::NODE, 0, 0));
  COLLADAFW::Node* nodeTwo = new COLLADAFW::Node(
      COLLADAFW::UniqueId(COLLADAFW::COLLADA_TYPE::NODE, 1, 0));
  visualScene->getRootNodes().append(nodeOne);
  visualScene->getRootNodes().append(nodeTwo);
  this->writer->writeVisualScene(visualScene);
  GLTF::Scene* scene = this->asset->getDefaultScene();
  ASSERT_TRUE(scene != NULL);
  std::vector<GLTF::Node*> sceneNodes = scene->nodes;
  EXPECT_EQ(sceneNodes.size(), 2);
}

TEST_F(COLLADA2GLTFWriterTest, WriteVisualScene_MeshDoesNotExist) {
  COLLADAFW::VisualScene* visualScene = new COLLADAFW::VisualScene(
      COLLADAFW::UniqueId(COLLADAFW::COLLADA_TYPE::VISUAL_SCENE, 0, 0));
  COLLADAFW::Node* node = new COLLADAFW::Node(
      COLLADAFW::UniqueId(COLLADAFW::COLLADA_TYPE::NODE, 0, 0));
  COLLADAFW::InstanceGeometry* instanceGeometry =
      new COLLADAFW::InstanceGeometry(
          COLLADAFW::UniqueId(COLLADAFW::COLLADA_TYPE::MESH, 0, 0),
          COLLADAFW::UniqueId(COLLADAFW::COLLADA_TYPE::MESH, 1, 0));
  node->getInstanceGeometries().append(instanceGeometry);
  visualScene->getRootNodes().append(node);
  this->writer->writeVisualScene(visualScene);
  GLTF::Scene* scene = this->asset->getDefaultScene();
  ASSERT_TRUE(scene != NULL);
  std::vector<GLTF::Node*> sceneNodes = scene->nodes;
  ASSERT_EQ(sceneNodes.size(), 1);
  GLTF::Node* sceneNode = sceneNodes[0];
  GLTF::Mesh* mesh = sceneNode->mesh;
  ASSERT_TRUE(mesh == NULL);
}

TEST_F(COLLADA2GLTFWriterTest, WriteVisualScene_MeshDoesExist) {
  COLLADAFW::VisualScene* visualScene = new COLLADAFW::VisualScene(
      COLLADAFW::UniqueId(COLLADAFW::COLLADA_TYPE::VISUAL_SCENE, 0, 0));
  COLLADAFW::Node* node = new COLLADAFW::Node(
      COLLADAFW::UniqueId(COLLADAFW::COLLADA_TYPE::NODE, 0, 0));
  COLLADAFW::InstanceGeometry* instanceGeometry =
      new COLLADAFW::InstanceGeometry(
          COLLADAFW::UniqueId(COLLADAFW::COLLADA_TYPE::MESH, 1, 0),
          COLLADAFW::UniqueId(COLLADAFW::COLLADA_TYPE::MESH, 0, 0));
  COLLADAFW::Geometry* geometry = new COLLADAFW::Mesh(
      COLLADAFW::UniqueId(COLLADAFW::COLLADA_TYPE::MESH, 0, 0));
  this->writer->writeGeometry(geometry);
  node->getInstanceGeometries().append(instanceGeometry);
  visualScene->getRootNodes().append(node);
  this->writer->writeVisualScene(visualScene);
  GLTF::Scene* scene = this->asset->getDefaultScene();
  ASSERT_TRUE(scene != NULL);
  std::vector<GLTF::Node*> sceneNodes = scene->nodes;
  ASSERT_EQ(sceneNodes.size(), 1);
  GLTF::Node* sceneNode = sceneNodes[0];
  GLTF::Mesh* mesh = sceneNode->mesh;
  ASSERT_TRUE(mesh != NULL);
}

TEST_F(COLLADA2GLTFWriterTest, WriteVisualScene_MultipleLevelsOfInstanceNodes) {
  COLLADAFW::VisualScene* visualScene = new COLLADAFW::VisualScene(
      COLLADAFW::UniqueId(COLLADAFW::COLLADA_TYPE::VISUAL_SCENE, 0, 0));

  // Create 2 root nodes
  COLLADAFW::Node* vsNode1 = new COLLADAFW::Node(
      COLLADAFW::UniqueId(COLLADAFW::COLLADA_TYPE::NODE, 0, 0));
  COLLADAFW::Node* vsNode2 = new COLLADAFW::Node(
      COLLADAFW::UniqueId(COLLADAFW::COLLADA_TYPE::NODE, 1, 0));
  visualScene->getRootNodes().append(vsNode1);
  visualScene->getRootNodes().append(vsNode2);

  // Create 2 instance_nodes that refers to the same node that is under
  // library_nodes
  COLLADAFW::UniqueId instanceId1(COLLADAFW::COLLADA_TYPE::NODE, 2, 0);
  COLLADAFW::InstanceNode* instance_libNode1_1 = new COLLADAFW::InstanceNode(
      COLLADAFW::UniqueId(COLLADAFW::COLLADA_TYPE::INSTANCE_NODE, 0, 0),
      instanceId1);
  COLLADAFW::InstanceNode* instance_libNode1_2 = new COLLADAFW::InstanceNode(
      COLLADAFW::UniqueId(COLLADAFW::COLLADA_TYPE::INSTANCE_NODE, 1, 0),
      instanceId1);
  vsNode1->getInstanceNodes().append(instance_libNode1_1);
  vsNode2->getInstanceNodes().append(instance_libNode1_2);

  // Create node under libary_node that is refered to by the 2 root nodes
  COLLADAFW::Node* libNode1 = new COLLADAFW::Node(instanceId1);

  // Create node under libary_node that is refered to by the previous node
  COLLADAFW::UniqueId instanceId2(COLLADAFW::COLLADA_TYPE::NODE, 3, 0);
  COLLADAFW::Node* libNode2 = new COLLADAFW::Node(instanceId2);

  COLLADAFW::InstanceNode* instance_libNode2 = new COLLADAFW::InstanceNode(
      COLLADAFW::UniqueId(COLLADAFW::COLLADA_TYPE::INSTANCE_NODE, 2, 0),
      instanceId2);
  libNode1->getInstanceNodes().append(instance_libNode2);

  COLLADAFW::LibraryNodes* libraryNodes = new COLLADAFW::LibraryNodes();
  libraryNodes->getNodes().append(libNode1);
  libraryNodes->getNodes().append(libNode2);

  this->writer->writeVisualScene(visualScene);
  this->writer->writeLibraryNodes(libraryNodes);

  GLTF::Scene* scene = this->asset->getDefaultScene();
  ASSERT_TRUE(scene != NULL);

  // We should have 2 root nodes
  std::vector<GLTF::Node*> sceneNodes = scene->nodes;
  ASSERT_EQ(sceneNodes.size(), 2);

  // Each of these nodes should have 1 child (clones of libNode1)
  ASSERT_EQ(sceneNodes[0]->children.size(), 1);
  ASSERT_EQ(sceneNodes[1]->children.size(), 1);

  // Each of those children should have 1 child (clones of libNode2)
  ASSERT_EQ(sceneNodes[0]->children[0]->children.size(), 1);
  ASSERT_EQ(sceneNodes[1]->children[0]->children.size(), 1);
}
