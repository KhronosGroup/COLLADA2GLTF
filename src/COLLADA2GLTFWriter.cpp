#include "COLLADA2GLTFWriter.h"

void COLLADA2GLTF::Writer::cancel(const std::string& errorMessage) {

}

void COLLADA2GLTF::Writer::start() {

}

void COLLADA2GLTF::Writer::finish() {

}

bool COLLADA2GLTF::Writer::writeGlobalAsset(const COLLADAFW::FileInfo* asset) {
	return true;
}

bool COLLADA2GLTF::Writer::writeNodeToGroup(std::vector<GLTF::Node*>* group, const COLLADAFW::Node* colladaNode) {
	GLTF::Asset* asset = this->_asset;
	std::map<COLLADAFW::UniqueId, GLTF::Mesh*> meshInstances = this->_meshInstances;
	GLTF::Node* node = new GLTF::Node();
	node->id = colladaNode->getOriginalId();
	node->name = colladaNode->getName();

	// Instance Geometries
	const COLLADAFW::InstanceGeometryPointerArray& instanceGeometries = colladaNode->getInstanceGeometries();
	size_t count = instanceGeometries.getCount();
	if (count > 0) {
		for (size_t i = 0; i < count; i++) {
			COLLADAFW::InstanceGeometry* instanceGeometry = instanceGeometries[i];
			const COLLADAFW::UniqueId& objectId = instanceGeometry->getInstanciatedObjectId();
			std::map<COLLADAFW::UniqueId, GLTF::Mesh*>::iterator iter = meshInstances.find(objectId);
			if (iter != meshInstances.end()) {
				GLTF::Mesh* mesh = iter->second;
				if (!node->meshes) {
					node->meshes = new std::vector<GLTF::Mesh*>();
				}
				node->meshes->push_back(mesh);
			}
		}
	}

	// Add to the group and recurse child nodes
	group->push_back(node);
	const COLLADAFW::NodePointerArray& childNodes = colladaNode->getChildNodes();
	if (childNodes.getCount() > 0) {
		return this->writeNodesToGroup(node->children, childNodes);
	}
	return true;
}

bool COLLADA2GLTF::Writer::writeNodesToGroup(std::vector<GLTF::Node*>* group, const COLLADAFW::NodePointerArray& nodes) {
	for (size_t i = 0; i < nodes.getCount(); i++) {
		if (!this->writeNodeToGroup(group, nodes[i])) {
			return false;
		}
	}
	return true;
}

bool COLLADA2GLTF::Writer::writeVisualScene(const COLLADAFW::VisualScene* visualScene) {
	GLTF::Asset* asset = this->_asset;
	GLTF::Scene* scene;
	if (asset->scene >= 0) {
		scene = new GLTF::Scene();
		asset->scenes->push_back(scene);
	}
	else {
		scene = asset->getDefaultScene();
	}
	return this->writeNodesToGroup(scene->nodes, visualScene->getRootNodes());
}

bool COLLADA2GLTF::Writer::writeScene(const COLLADAFW::Scene* scene) {
	return true;
}

bool COLLADA2GLTF::Writer::writeLibraryNodes(const COLLADAFW::LibraryNodes* libraryNodes) {
	GLTF::Asset* asset = this->_asset;
	GLTF::Scene* scene = asset->getDefaultScene();
	if (!scene->nodes) {
		scene->nodes = new std::vector<GLTF::Node*>();
	}
	return this->writeNodesToGroup(scene->nodes, libraryNodes->getNodes());
}

bool COLLADA2GLTF::Writer::writePrimitiveToMesh(GLTF::Mesh* mesh, const COLLADAFW::MeshPrimitive* colladaPrimitive) {
	GLTF::Primitive* primitive = new GLTF::Primitive();
	switch (colladaPrimitive->getPrimitiveType()) {
	case COLLADAFW::MeshPrimitive::TRIANGLES: {
		primitive->mode = GLTF::Primitive::Mode::TRIANGLES;
		break;
	}
	case COLLADAFW::MeshPrimitive::POLYLIST: {}
	case COLLADAFW::MeshPrimitive::POLYGONS: {}
	case COLLADAFW::MeshPrimitive::LINES: {}
	}
	return true;
}

bool COLLADA2GLTF::Writer::writeMesh(const COLLADAFW::Mesh* colladaMesh) {
	GLTF::Mesh* mesh = new GLTF::Mesh();
	mesh->id = colladaMesh->getOriginalId();
	mesh->name = colladaMesh->getName();

	const COLLADAFW::MeshPrimitiveArray& primitives = colladaMesh->getMeshPrimitives();
	size_t primitivesCount = primitives.getCount();
	for (size_t i = 0; i < primitivesCount; i++) {
		if (!this->writePrimitiveToMesh(mesh, primitives[i])) {
			return false;
		}
	}
	return true;
}

bool COLLADA2GLTF::Writer::writeGeometry(const COLLADAFW::Geometry* geometry) {
	switch (geometry->getType()) {
	case COLLADAFW::Geometry::GEO_TYPE_MESH: {
		const COLLADAFW::Mesh* mesh = (COLLADAFW::Mesh*)geometry;
		if (!writeMesh(mesh)) {
			return false;
		}
		break;
	}
	default: {
		return false;
	}}
	return true;
}

bool COLLADA2GLTF::Writer::writeMaterial(const COLLADAFW::Material* material) {
	return true;
}

bool COLLADA2GLTF::Writer::writeEffect(const COLLADAFW::Effect* effect) {
	return true;
}

bool COLLADA2GLTF::Writer::writeCamera(const COLLADAFW::Camera* camera) {
	return true;
}

bool COLLADA2GLTF::Writer::writeImage(const COLLADAFW::Image* image) {
	return true;
}

bool COLLADA2GLTF::Writer::writeLight(const COLLADAFW::Light* light) {
	return true;
}

bool COLLADA2GLTF::Writer::writeAnimation(const COLLADAFW::Animation* animation) {
	return true;
}

bool COLLADA2GLTF::Writer::writeAnimationList(const COLLADAFW::AnimationList* animationList) {
	return true;
}

bool COLLADA2GLTF::Writer::writeSkinControllerData(const COLLADAFW::SkinControllerData* skinControllerData) {
	return true;
}

bool COLLADA2GLTF::Writer::writeController(const COLLADAFW::Controller* Controller) {
	return true;
}

bool COLLADA2GLTF::Writer::writeFormulas(const COLLADAFW::Formulas* formulas) {
	return true;
}

bool COLLADA2GLTF::Writer::writeKinematicsScene(const COLLADAFW::KinematicsScene* kinematicsScene) {
	return true;
}
