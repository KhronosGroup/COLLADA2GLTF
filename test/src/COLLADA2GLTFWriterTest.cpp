#include <vector>

#include "COLLADA2GLTFWriterTest.h"
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
	COLLADAFW::VisualScene* visualScene = new COLLADAFW::VisualScene(COLLADAFW::UniqueId(COLLADAFW::COLLADA_TYPE::VISUAL_SCENE, 0, 0));
	COLLADAFW::Node* node = new COLLADAFW::Node(COLLADAFW::UniqueId(COLLADAFW::COLLADA_TYPE::NODE, 0, 0));
	visualScene->getRootNodes().append(node);
	this->writer->writeVisualScene(visualScene);
	GLTF::Scene* scene = this->asset->getDefaultScene();
	ASSERT_TRUE(scene != NULL);
	std::vector<GLTF::Node*> sceneNodes = scene->nodes;
	EXPECT_EQ(sceneNodes.size(), 1);
}

TEST_F(COLLADA2GLTFWriterTest, WriteVisualScene_MultipleNodes) {
	COLLADAFW::VisualScene* visualScene = new COLLADAFW::VisualScene(COLLADAFW::UniqueId(COLLADAFW::COLLADA_TYPE::VISUAL_SCENE, 0, 0));
	COLLADAFW::Node* nodeOne = new COLLADAFW::Node(COLLADAFW::UniqueId(COLLADAFW::COLLADA_TYPE::NODE, 0, 0));
	COLLADAFW::Node* nodeTwo = new COLLADAFW::Node(COLLADAFW::UniqueId(COLLADAFW::COLLADA_TYPE::NODE, 1, 0));
	visualScene->getRootNodes().append(nodeOne);
	visualScene->getRootNodes().append(nodeTwo);
	this->writer->writeVisualScene(visualScene);
	GLTF::Scene* scene = this->asset->getDefaultScene();
	ASSERT_TRUE(scene != NULL);
	std::vector<GLTF::Node*> sceneNodes = scene->nodes;
	EXPECT_EQ(sceneNodes.size(), 2);
}

TEST_F(COLLADA2GLTFWriterTest, WriteVisualScene_MeshDoesNotExist) {
	COLLADAFW::VisualScene* visualScene = new COLLADAFW::VisualScene(COLLADAFW::UniqueId(COLLADAFW::COLLADA_TYPE::VISUAL_SCENE, 0, 0));
	COLLADAFW::Node* node = new COLLADAFW::Node(COLLADAFW::UniqueId(COLLADAFW::COLLADA_TYPE::NODE, 0, 0));
	COLLADAFW::InstanceGeometry* instanceGeometry = new COLLADAFW::InstanceGeometry(
		COLLADAFW::UniqueId(COLLADAFW::COLLADA_TYPE::MESH, 0, 0),
		COLLADAFW::UniqueId(COLLADAFW::COLLADA_TYPE::MESH, 1, 0)
	);
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
	COLLADAFW::VisualScene* visualScene = new COLLADAFW::VisualScene(COLLADAFW::UniqueId(COLLADAFW::COLLADA_TYPE::VISUAL_SCENE, 0, 0));
	COLLADAFW::Node* node = new COLLADAFW::Node(COLLADAFW::UniqueId(COLLADAFW::COLLADA_TYPE::NODE, 0, 0));
	COLLADAFW::InstanceGeometry* instanceGeometry = new COLLADAFW::InstanceGeometry(
		COLLADAFW::UniqueId(COLLADAFW::COLLADA_TYPE::MESH, 1, 0),
		COLLADAFW::UniqueId(COLLADAFW::COLLADA_TYPE::MESH, 0, 0)
	);
	COLLADAFW::Geometry* geometry = new COLLADAFW::Mesh(COLLADAFW::UniqueId(COLLADAFW::COLLADA_TYPE::MESH, 0, 0));
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
