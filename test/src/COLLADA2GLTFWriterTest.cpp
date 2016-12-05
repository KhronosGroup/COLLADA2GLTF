#include <vector>

#include "COLLADA2GLTFWriterTest.h"
#include "COLLADABU.h"
#include "COLLADAFW.h"

COLLADA2GLTFWriterTest::COLLADA2GLTFWriterTest() {
	asset = new GLTF::Asset();
	options = new COLLADA2GLTF::Options();
	writer = new COLLADA2GLTF::Writer(asset, options);
}

COLLADA2GLTFWriterTest::~COLLADA2GLTFWriterTest() {
	delete writer;
	delete asset;
}

TEST_F(COLLADA2GLTFWriterTest, WriteLibraryNodes_SingleNode) {
	COLLADAFW::LibraryNodes* nodes = new COLLADAFW::LibraryNodes();
	COLLADAFW::Node* node = new COLLADAFW::Node(COLLADAFW::UniqueId(COLLADAFW::COLLADA_TYPE::NODE, 0, 0));
	nodes->getNodes().append(node);
	this->writer->writeLibraryNodes(nodes);
	GLTF::Scene* scene = this->asset->getDefaultScene();
	ASSERT_TRUE(scene != NULL);
	std::vector<GLTF::Node*> sceneNodes = scene->nodes;
	EXPECT_EQ(sceneNodes.size(), 1);
}

TEST_F(COLLADA2GLTFWriterTest, WriteLibraryNodes_MultipleNodes) {
	COLLADAFW::LibraryNodes* nodes = new COLLADAFW::LibraryNodes();
	COLLADAFW::Node* nodeOne = new COLLADAFW::Node(COLLADAFW::UniqueId(COLLADAFW::COLLADA_TYPE::NODE, 0, 0));
	COLLADAFW::Node* nodeTwo = new COLLADAFW::Node(COLLADAFW::UniqueId(COLLADAFW::COLLADA_TYPE::NODE, 1, 0));
	nodes->getNodes().append(nodeOne);
	nodes->getNodes().append(nodeTwo);
	this->writer->writeLibraryNodes(nodes);
	GLTF::Scene* scene = this->asset->getDefaultScene();
	ASSERT_TRUE(scene != NULL);
	std::vector<GLTF::Node*> sceneNodes = scene->nodes;
	EXPECT_EQ(sceneNodes.size(), 2);
}

TEST_F(COLLADA2GLTFWriterTest, WriteLibraryNodes_MeshDoesNotExist) {
	COLLADAFW::LibraryNodes* nodes = new COLLADAFW::LibraryNodes();
	COLLADAFW::Node* node = new COLLADAFW::Node(COLLADAFW::UniqueId(COLLADAFW::COLLADA_TYPE::NODE, 0, 0));
	COLLADAFW::InstanceGeometry* instanceGeometry = new COLLADAFW::InstanceGeometry(
		COLLADAFW::UniqueId(COLLADAFW::COLLADA_TYPE::MESH, 0, 0),
		COLLADAFW::UniqueId(COLLADAFW::COLLADA_TYPE::MESH, 1, 0)
	);
	node->getInstanceGeometries().append(instanceGeometry);
	nodes->getNodes().append(node);
	this->writer->writeLibraryNodes(nodes);
	GLTF::Scene* scene = this->asset->getDefaultScene();
	ASSERT_TRUE(scene != NULL);
	std::vector<GLTF::Node*> sceneNodes = scene->nodes;
	ASSERT_EQ(sceneNodes.size(), 1);
	GLTF::Node* sceneNode = sceneNodes[0];
	std::vector<GLTF::Mesh*> meshes = sceneNode->meshes;
	ASSERT_EQ(meshes.size(), 0);
}

TEST_F(COLLADA2GLTFWriterTest, WriteLibraryNodes_MeshDoesExist) {
	COLLADAFW::LibraryNodes* nodes = new COLLADAFW::LibraryNodes();
	COLLADAFW::Node* node = new COLLADAFW::Node(COLLADAFW::UniqueId(COLLADAFW::COLLADA_TYPE::NODE, 0, 0));
	COLLADAFW::InstanceGeometry* instanceGeometry = new COLLADAFW::InstanceGeometry(
		COLLADAFW::UniqueId(COLLADAFW::COLLADA_TYPE::MESH, 0, 0),
		COLLADAFW::UniqueId(COLLADAFW::COLLADA_TYPE::MESH, 1, 0)
	);
	COLLADAFW::Geometry* geometry = new COLLADAFW::Mesh(COLLADAFW::UniqueId(COLLADAFW::COLLADA_TYPE::MESH, 0, 0));
	this->writer->writeGeometry(geometry);
	node->getInstanceGeometries().append(instanceGeometry);
	nodes->getNodes().append(node);
	this->writer->writeLibraryNodes(nodes);
	GLTF::Scene* scene = this->asset->getDefaultScene();
	ASSERT_TRUE(scene != NULL);
	std::vector<GLTF::Node*> sceneNodes = scene->nodes;
	ASSERT_EQ(sceneNodes.size(), 1);
	GLTF::Node* sceneNode = sceneNodes[0];
	std::vector<GLTF::Mesh*> meshes = sceneNode->meshes;
	ASSERT_EQ(meshes.size(), 1);
}
