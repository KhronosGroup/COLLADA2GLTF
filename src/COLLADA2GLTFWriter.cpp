#include "COLLADA2GLTFWriter.h"

#include <experimental/filesystem>

#include "Base64.h"

using namespace std::experimental::filesystem;

COLLADA2GLTF::Writer::Writer(GLTF::Asset* asset, COLLADA2GLTF::Options* options, COLLADA2GLTF::ExtrasHandler* extrasHandler) : _asset(asset), _options(options), _extrasHandler(extrasHandler) {
}

void COLLADA2GLTF::Writer::cancel(const std::string& errorMessage) {

}

void COLLADA2GLTF::Writer::start() {

}

void COLLADA2GLTF::Writer::finish() {

}

bool COLLADA2GLTF::Writer::writeGlobalAsset(const COLLADAFW::FileInfo* asset) {
	if (asset->getUpAxisType() == COLLADAFW::FileInfo::X_UP) {
		_rootNode = new GLTF::Node();
		_rootNode->transform = new GLTF::Node::TransformMatrix(
			0, -1, 0, 0,
			1, 0, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		);
		_rootNode->name = "X_UP";
	} else if (asset->getUpAxisType() == COLLADAFW::FileInfo::Z_UP) {
		_rootNode = new GLTF::Node();
		_rootNode->transform = new GLTF::Node::TransformMatrix(
			1, 0, 0, 0,
			0, 0, 1, 0,
			0, -1, 0, 0,
			0, 0, 0, 1
		);
		_rootNode->name = "Z_UP";
	}
	return true;
}

COLLADABU::Math::Matrix4 getMatrixFromTransform(const COLLADAFW::Transformation* transform) {
	switch (transform->getTransformationType()) {
	case COLLADAFW::Transformation::ROTATE: {
		COLLADAFW::Rotate* rotate = (COLLADAFW::Rotate*)transform;
		COLLADABU::Math::Vector3 axis = rotate->getRotationAxis();
		axis.normalise();
		double angle = rotate->getRotationAngle();
		return COLLADABU::Math::Matrix4(COLLADABU::Math::Quaternion(COLLADABU::Math::Utils::degToRad(angle), axis));
	}
	case COLLADAFW::Transformation::TRANSLATE: {
		COLLADAFW::Translate* translate = (COLLADAFW::Translate*)transform;
		const COLLADABU::Math::Vector3& translation = translate->getTranslation();
		COLLADABU::Math::Matrix4 translationMatrix;
		translationMatrix.makeTrans(translation);
		return translationMatrix;
	}
	case COLLADAFW::Transformation::SCALE: {
		COLLADAFW::Scale* scale = (COLLADAFW::Scale*)transform;
		const COLLADABU::Math::Vector3& scaleVector = scale->getScale();
		COLLADABU::Math::Matrix4 scaleMatrix;
		scaleMatrix.makeScale(scaleVector);
		return scaleMatrix;
	}
	case COLLADAFW::Transformation::MATRIX: {
		COLLADAFW::Matrix* transformMatrix = (COLLADAFW::Matrix*)transform;
		return transformMatrix->getMatrix();
	}
	case COLLADAFW::Transformation::LOOKAT: {
		COLLADAFW::Lookat* lookAt = (COLLADAFW::Lookat*)transform;
		const COLLADABU::Math::Vector3& eye = lookAt->getEyePosition();
		const COLLADABU::Math::Vector3& center = lookAt->getInterestPointPosition();
		const COLLADABU::Math::Vector3& up = lookAt->getUpAxisDirection();
		COLLADABU::Math::Matrix4 lookAtMatrix = COLLADABU::Math::Matrix4::IDENTITY;
		if ((eye.x != center.x) || (eye.y != center.y) || (eye.z != center.z)) {
			COLLADABU::Math::Vector3 z = (eye - center);
			z.normalise();
			COLLADABU::Math::Vector3 x = up.crossProduct(z);
			x.normalise();
			COLLADABU::Math::Vector3 y = z.crossProduct(x);
			y.normalise();
			lookAtMatrix.setAllElements(
				x.x, y.x, z.x, 0,
				x.y, y.y, z.y, 0,
				x.z, y.z, z.z, 0,
				-(x.x * eye.x + x.y  * eye.y + x.z * eye.z),
				-(y.x * eye.x + y.y * eye.y + y.z * eye.z),
				-(z.x * eye.x + z.y * eye.y + z.z * eye.z),
				1);
			lookAtMatrix = lookAtMatrix.inverse();
			lookAtMatrix = lookAtMatrix.transpose();
		}
		return lookAtMatrix;
	}}
	return COLLADABU::Math::Matrix4::IDENTITY;
}

COLLADABU::Math::Matrix4 getFlattenedTransform(std::vector<const COLLADAFW::Transformation*> transforms) {
	COLLADABU::Math::Matrix4 matrix = COLLADABU::Math::Matrix4::IDENTITY;
	for (const COLLADAFW::Transformation* transform : transforms) {
		matrix = matrix * getMatrixFromTransform(transform);
	}
	return matrix;
}

void packColladaMatrix(COLLADABU::Math::Matrix4 matrix, float* dataArray, size_t offset) {
	for (int j = 0; j < 4; j++) {
		for (int k = 0; k < 4; k++) {
			dataArray[k * 4 + j + offset] = (float)matrix.getElement(j, k);
		}
	}
}

void packColladaMatrix(COLLADABU::Math::Matrix4 matrix, GLTF::Node::TransformMatrix* transform) {
	packColladaMatrix(matrix, transform->matrix, 0);
}

bool COLLADA2GLTF::Writer::writeNodeToGroup(std::vector<GLTF::Node*>* group, const COLLADAFW::Node* colladaNode) {
	GLTF::Asset* asset = this->_asset;
	GLTF::Node* node = new GLTF::Node();
	COLLADABU::Math::Matrix4 matrix;
	GLTF::Node::TransformMatrix* transform;
	// Add root node to group
	group->push_back(node);
	const COLLADAFW::UniqueId& colladaNodeId = colladaNode->getUniqueId();
	std::string id = colladaNode->getOriginalId();
	node->name = colladaNode->getName();
	if (node->name == "") {
		node->name = id;
	}
	COLLADAFW::TransformationPointerArray transformations = colladaNode->getTransformations();

	std::vector<const COLLADAFW::Transformation*> nodeTransforms;
	bool isAnimated = false;
	int bufferNodes = 0;
	for (size_t i = 0; i < transformations.getCount(); i++) {
		const COLLADAFW::Transformation* transformation = transformations[i];
		const COLLADAFW::UniqueId& animationListId = transformation->getAnimationList();
		if (animationListId.isValid()) {
			if (nodeTransforms.size() > 0) {
				// If the current top level node is animated, we need to make a buffer node so the transform is not changed
				if (isAnimated) {
					GLTF::Node* bufferNode = new GLTF::Node();
					bufferNodes++;
					node->children.push_back(bufferNode);
					node = bufferNode;
				}

				// Any prior node transforms get flattened out onto the last node
				COLLADABU::Math::Matrix4 matrix = COLLADABU::Math::Matrix4::IDENTITY;
				matrix = getFlattenedTransform(nodeTransforms);
				transform = new GLTF::Node::TransformMatrix();
				packColladaMatrix(matrix, transform);
				node->transform = transform;
				nodeTransforms.clear();

				GLTF::Node* animatedNode = new GLTF::Node();
				// These animated nodes are assigned the same ids initially, and a suffix is added when the animation gets applied
				node->children.push_back(animatedNode);
				node = animatedNode;
			}

			// The animated node has the current transformation
			matrix = getMatrixFromTransform(transformation);
			transform = new GLTF::Node::TransformMatrix();
			packColladaMatrix(matrix, transform);
			node->transform = transform;

			_animatedNodes[animationListId] = node;
			isAnimated = true;
		}
		else {
			nodeTransforms.push_back(transformation);
		}
	}
	// Write out remaining transforms onto top-level node
	matrix = COLLADABU::Math::Matrix4::IDENTITY;
	if (nodeTransforms.size() > 0) {
		// If the current top level node is animated, we need to make a buffer node so the transform is not changed
		matrix = getFlattenedTransform(nodeTransforms);
		if (matrix != COLLADABU::Math::Matrix4::IDENTITY) {
			if (isAnimated) {
				GLTF::Node* bufferNode = new GLTF::Node();
				bufferNodes++;
				node->children.push_back(bufferNode);
				node = bufferNode;
			}
		}
	}
	transform = new GLTF::Node::TransformMatrix();
	packColladaMatrix(matrix, transform);
	if (node->transform == NULL) {
		node->transform = transform;
	}

	// Identify and map joint nodes
	for (auto const& skinNodes : _skinJointNodes) {
		const COLLADAFW::UniqueId& skinId = skinNodes.first;
		GLTF::Skin* skin = _skinInstances[skinId];
		std::vector<COLLADAFW::UniqueId> nodeIds = skinNodes.second;
		for (size_t i = 0; i < nodeIds.size(); i++) {
			if (nodeIds[i] == colladaNodeId) {
				while (i >= skin->joints.size()) {
					skin->joints.push_back(NULL);
				}
				skin->joints[i] = node;
				node->jointName = id;
				break;
			}
		}
	}

	// Instance skinning
	const COLLADAFW::InstanceControllerPointerArray& instanceControllers = colladaNode->getInstanceControllers();
	for (size_t i = 0; i < instanceControllers.getCount(); i++) {
		COLLADAFW::InstanceController* instanceController = instanceControllers[i];
		COLLADAFW::UniqueId uniqueId = instanceController->getInstanciatedObjectId();
		std::map<COLLADAFW::UniqueId, GLTF::Skin*>::iterator iter = _skinInstances.find(uniqueId);
		if (iter != _skinInstances.end()) {
			GLTF::Skin* skin = iter->second;
			node->skin = skin;

			GLTF::Mesh* skinnedMesh = _skinnedMeshes[uniqueId];
			GLTF::Node* skinnedMeshNode = new GLTF::Node();
			skinnedMeshNode->mesh = skinnedMesh;
			node->children.push_back(skinnedMeshNode);

			const COLLADAFW::MaterialBindingArray &materialBindings = instanceController->getMaterialBindings();
			for (size_t j = 0; j < materialBindings.getCount(); j++) {
				COLLADAFW::MaterialBinding materialBinding = materialBindings[j];
				GLTF::Primitive* primitive = skinnedMesh->primitives[j];
				COLLADAFW::UniqueId materialId = materialBinding.getReferencedMaterial();
				COLLADAFW::UniqueId effectId = this->_materialEffects[materialId];
				GLTF::Material* material = _effectInstances[effectId];
				if (material->type == GLTF::Material::Type::MATERIAL_COMMON) {
					GLTF::MaterialCommon* materialCommon = (GLTF::MaterialCommon*)material;
					materialCommon->jointCount = _skinJointNodes[uniqueId].size();
				}
				primitive->material = material;
			}

			for (const COLLADABU::URI& skeletonURI : instanceController->skeletons()) {
				std::string skeletonId = skeletonURI.getFragment();
				std::map<std::string, GLTF::Node*>::iterator iter = _nodes.find(skeletonId);
				if (iter != _nodes.end()) {
					node->skeletons.push_back(iter->second);
				} else {
					// The skeleton node hasn't been created yet, mark it as unbound
					GLTF::Node* unboundNodePointer = NULL;
					_unboundSkeletonNodes[skeletonId] = &node->skeletons;
				}
			}
		}
	}

	// Instance lights
	const COLLADAFW::InstanceLightPointerArray& instanceLights = colladaNode->getInstanceLights();
	for (size_t i = 0; i < instanceLights.getCount(); i++) {
		COLLADAFW::InstanceLight* instanceLight = instanceLights[i];
		GLTF::MaterialCommon::Light* light = _lightInstances[instanceLight->getInstanciatedObjectId()];
		node->light = light;
		light->node = node;
	}

	// Instance cameras
	const COLLADAFW::InstanceCameraPointerArray& instanceCameras = colladaNode->getInstanceCameras();
	for (size_t i = 0; i < instanceCameras.getCount(); i++) {
		COLLADAFW::InstanceCamera* instanceCamera = instanceCameras[i];
		GLTF::Camera* camera = _cameraInstances[instanceCamera->getInstanciatedObjectId()];
		node->camera = camera;
	}

	// Identify and map unbound skeleton nodes
	if (_unboundSkeletonNodes.size() > 0) {
		std::map<std::string, std::vector<GLTF::Node*>*>::iterator iter = _unboundSkeletonNodes.find(id);
		if (iter != _unboundSkeletonNodes.end()) {
			std::vector<GLTF::Node*>* skeletonNodes = iter->second;
			skeletonNodes->push_back(node);
			_unboundSkeletonNodes.erase(iter);
		}
	}

	// Instance Geometries
	const COLLADAFW::InstanceGeometryPointerArray& instanceGeometries = colladaNode->getInstanceGeometries();
	size_t count = instanceGeometries.getCount();
	if (count > 0) {
		for (size_t i = 0; i < count; i++) {
			COLLADAFW::InstanceGeometry* instanceGeometry = instanceGeometries[i];
			COLLADAFW::MaterialBindingArray& materialBindings = instanceGeometry->getMaterialBindings();
			const COLLADAFW::UniqueId& objectId = instanceGeometry->getInstanciatedObjectId();
			std::map<int, std::set<GLTF::Primitive*>> primitiveMaterialMapping = _meshMaterialPrimitiveMapping[objectId];
			std::map<COLLADAFW::UniqueId, GLTF::Mesh*>::iterator iter = _meshInstances.find(objectId);
			if (iter != _meshInstances.end()) {
				GLTF::Mesh* mesh = iter->second;
				for (size_t j = 0; j < materialBindings.getCount(); j++) {
					COLLADAFW::MaterialBinding materialBinding = materialBindings[j];
					COLLADAFW::UniqueId materialId = materialBinding.getReferencedMaterial();
					COLLADAFW::UniqueId effectId = this->_materialEffects[materialId];
					GLTF::Material* material = _effectInstances[effectId];
					for (GLTF::Primitive* primitive : primitiveMaterialMapping[materialBinding.getMaterialId()]) {
						if (primitive->material != NULL && primitive->material != material) {
							// This mesh primitive has a different material from a previous instance, clone the mesh and primitives
							mesh = (GLTF::Mesh*)mesh->clone();
							primitive = mesh->primitives[j];
						}
						primitive->material = material;
					}
				}
				if (node->mesh != NULL) {
					// Split out a new child node if this one already has a mesh
					GLTF::Node* meshNode = new GLTF::Node();
					meshNode->mesh = mesh;
					node->children.push_back(meshNode);
				}
				else {
					node->mesh = mesh;
				}
			}
		}
	}
	_nodes[id] = node;

	// Recurse child nodes
	const COLLADAFW::NodePointerArray& childNodes = colladaNode->getChildNodes();
	if (childNodes.getCount() > 0) {
		return this->writeNodesToGroup(&node->children, childNodes);
	}
	return true;
}

bool COLLADA2GLTF::Writer::writeNodesToGroup(std::vector<GLTF::Node*>* group, const COLLADAFW::NodePointerArray& nodes) {
	for (unsigned int i = 0; i < nodes.getCount(); i++) {
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
		asset->scenes.push_back(scene);
	}
	else {
		scene = asset->getDefaultScene();
	}
	if (_rootNode != NULL) {
		scene->nodes.push_back(_rootNode);
		return this->writeNodesToGroup(&_rootNode->children, visualScene->getRootNodes());
	}
	return this->writeNodesToGroup(&scene->nodes, visualScene->getRootNodes());
}

bool COLLADA2GLTF::Writer::writeScene(const COLLADAFW::Scene* scene) {
	return true;
}

bool COLLADA2GLTF::Writer::writeLibraryNodes(const COLLADAFW::LibraryNodes* libraryNodes) {
	GLTF::Asset* asset = this->_asset;
	GLTF::Scene* scene = asset->getDefaultScene();
	return this->writeNodesToGroup(&scene->nodes, libraryNodes->getNodes());
}

void mapAttributeIndices(const unsigned int* rootIndices, const unsigned* indices, int count, std::string semantic, std::map<std::string, GLTF::Accessor*>* attributes, std::map<std::string, std::map<int, int>>* indicesMapping) {
	indicesMapping->emplace(semantic, std::map<int, int>());
	for (int i = 0; i < count; i++) {
		unsigned int rootIndex = rootIndices[i];
		unsigned int index = indices[i];
		if (rootIndex != index) {
			indicesMapping->at(semantic).emplace(rootIndex, index);
		}
	}
	attributes->emplace(semantic, (GLTF::Accessor*)NULL);
}

void mapAttributeIndicesArray(const unsigned int* rootIndices, const COLLADAFW::IndexListArray& indicesArray, int count, std::string baseSemantic, std::map<std::string, GLTF::Accessor*>* attributes, std::map<std::string, std::map<int, int>>* indicesMapping) {
	int indicesArrayCount = indicesArray.getCount();
	for (int i = 0; i < indicesArrayCount; i++) {
		std::string semantic = baseSemantic;
		if (indicesArrayCount > 1) {
			semantic += "_" + std::to_string(i);
		}
		mapAttributeIndices(rootIndices, indicesArray[i]->getIndices().getData(), count, semantic, attributes, indicesMapping);
	}
}

GLTF::Accessor* bufferAndMapVertexData(GLTF::BufferView* bufferView, GLTF::Accessor::Type type, const COLLADAFW::MeshVertexData& vertexData, std::map<int, int> indicesMapping) {
	int count = vertexData.getValuesCount();
	float* floatBuffer = new float[count];
	COLLADAFW::FloatOrDoubleArray::DataType dataType = vertexData.getType();
	for (int i = 0; i < count; i++) {
		int index = i;
		std::map<int, int>::iterator mappedIndex = indicesMapping.find(index);
		if (mappedIndex != indicesMapping.end()) {
			index = mappedIndex->second;
		}
		switch (dataType) {
		case COLLADAFW::FloatOrDoubleArray::DATA_TYPE_DOUBLE:
			floatBuffer[index] = (float)(vertexData.getDoubleValues()->getData()[i]);
			break;
		case COLLADAFW::FloatOrDoubleArray::DATA_TYPE_FLOAT:
			floatBuffer[index] = vertexData.getFloatValues()->getData()[i];
			break;
		default:
			free(floatBuffer);
			return NULL;
		}
	}
	GLTF::Accessor* accessor = new GLTF::Accessor(type, GLTF::Constants::WebGL::FLOAT, (unsigned char*)floatBuffer, count / GLTF::Accessor::getNumberOfComponents(type), bufferView);
	free(floatBuffer);
	return accessor;
}

float getMeshVertexDataAtIndex(const COLLADAFW::MeshVertexData& data, int index) {
	COLLADAFW::FloatOrDoubleArray::DataType type = data.getType();
	if (type == COLLADAFW::FloatOrDoubleArray::DATA_TYPE_DOUBLE) {
		return (float)data.getDoubleValues()->getData()[index];
	}
	return data.getFloatValues()->getData()[index];
}

std::string buildAttributeId(const COLLADAFW::MeshVertexData& data, int index, int count) {
	std::string id;
	for (int i = 0; i < count; i++) {
		id += std::to_string(getMeshVertexDataAtIndex(data, index * count + i)) + ":";
	}
	return id;
}

/**
 * Converts and writes a <COLLADAFW::Mesh> to a <GLTF::Mesh>.
 * The produced meshes are stored in `this->_meshInstances` indexed by their <COLLADAFW::UniqueId>.
 *
 * COLLADA has different sets of indices per attribute in primitives while glTF uses a single indices
 * accessor for a primitive and requires attributes to be aligned. Attributes are built using the
 * the COLLADA indices, and duplicate attributes are referenced by index.
 *
 * @param colladaMesh The COLLADA mesh to write to glTF
 * @return `true` if the operation completed succesfully, `false` if an error occured
 */
bool COLLADA2GLTF::Writer::writeMesh(const COLLADAFW::Mesh* colladaMesh) {
	GLTF::Mesh* mesh = new GLTF::Mesh();
	mesh->name = colladaMesh->getName();
	const COLLADAFW::UniqueId& uniqueId = colladaMesh->getUniqueId();
	std::map<GLTF::Primitive*, std::vector<int>> positionMapping;

	const COLLADAFW::MeshPrimitiveArray& meshPrimitives = colladaMesh->getMeshPrimitives();
	std::map<int, std::set<GLTF::Primitive*>> primitiveMaterialMapping;
	int meshPrimitivesCount = meshPrimitives.getCount();
	if (meshPrimitivesCount > 0) {
		// Create primitives
		for (int i = 0; i < meshPrimitivesCount; i++) {
			std::map<std::string, std::vector<float>> buildAttributes;
			std::map<std::string, unsigned short> attributeIndicesMapping;
			std::vector<unsigned short> buildIndices;
			COLLADAFW::MeshPrimitive* colladaPrimitive = meshPrimitives[i];
			GLTF::Primitive* primitive = new GLTF::Primitive();

			int materialId = colladaPrimitive->getMaterialId();
			std::map<int, std::set<GLTF::Primitive*>>::iterator findPrimitiveSet = primitiveMaterialMapping.find(materialId);
			if (findPrimitiveSet != primitiveMaterialMapping.end()) {
				findPrimitiveSet->second.insert(primitive);
			}
			else {
				std::set<GLTF::Primitive*> primitiveSet;
				primitiveSet.insert(primitive);
				primitiveMaterialMapping[materialId] = primitiveSet;
			}

			std::vector<int> mapping;
			bool shouldTriangulate = false;

			COLLADAFW::MeshPrimitive::PrimitiveType type = colladaPrimitive->getPrimitiveType();
			switch (colladaPrimitive->getPrimitiveType()) {
			case COLLADAFW::MeshPrimitive::LINES:
				primitive->mode = GLTF::Primitive::Mode::LINES;
				break;
			case COLLADAFW::MeshPrimitive::LINE_STRIPS:
				primitive->mode = GLTF::Primitive::Mode::LINE_STRIP;
				break;
			// Having POLYLIST and POLYGONS map to TRIANGLES produces good output for cases where the polygons are already triangles, 
			// but in other cases, we may need to triangulate
			case COLLADAFW::MeshPrimitive::POLYLIST:
			case COLLADAFW::MeshPrimitive::POLYGONS:
				shouldTriangulate = true;
			case COLLADAFW::MeshPrimitive::TRIANGLES:
				primitive->mode = GLTF::Primitive::Mode::TRIANGLES;
				break;
			case COLLADAFW::MeshPrimitive::TRIANGLE_STRIPS:
				primitive->mode = GLTF::Primitive::Mode::TRIANGLE_STRIP;
				break;
			case COLLADAFW::MeshPrimitive::TRIANGLE_FANS:
				primitive->mode = GLTF::Primitive::Mode::TRIANGLE_FAN;
				break;
			case COLLADAFW::MeshPrimitive::POINTS:
				primitive->mode = GLTF::Primitive::Mode::POINTS;
				break;
				primitive->mode = GLTF::Primitive::Mode::TRIANGLES;
				break;
			}

			if (primitive->mode == GLTF::Primitive::Mode::UNKNOWN) {
				continue;
			}
			int count = colladaPrimitive->getPositionIndices().getCount();
			std::map<std::string, const unsigned int*> semanticIndices;
			std::map<std::string, const COLLADAFW::MeshVertexData*> semanticData;
			std::string semantic = "POSITION";
			buildAttributes[semantic] = std::vector<float>();
			semanticIndices[semantic] = colladaPrimitive->getPositionIndices().getData();
			semanticData[semantic] = &colladaMesh->getPositions();
			primitive->attributes[semantic] = (GLTF::Accessor*)NULL;
			if (colladaPrimitive->hasNormalIndices()) {
				semantic = "NORMAL";
				buildAttributes[semantic] = std::vector<float>();
				semanticIndices[semantic] = colladaPrimitive->getNormalIndices().getData();
				semanticData[semantic] = &colladaMesh->getNormals();
				primitive->attributes[semantic] = (GLTF::Accessor*)NULL;
			}
			if (colladaPrimitive->hasBinormalIndices()) {
				semantic = "BINORMAL";
				buildAttributes[semantic] = std::vector<float>();
				semanticIndices[semantic] = colladaPrimitive->getBinormalIndices().getData();
				semanticData[semantic] = &colladaMesh->getBinormals();
				primitive->attributes[semantic] = (GLTF::Accessor*)NULL;
			}
			if (colladaPrimitive->hasTangentIndices()) {
				semantic = "TANGENT";
;				buildAttributes[semantic] = std::vector<float>();
				semanticIndices[semantic] = colladaPrimitive->getTangentIndices().getData();
				semanticData[semantic] = &colladaMesh->getTangents();
				primitive->attributes[semantic] = (GLTF::Accessor*)NULL;
			}
			if (colladaPrimitive->hasUVCoordIndices()) {
				COLLADAFW::IndexListArray& uvCoordIndicesArray = colladaPrimitive->getUVCoordIndicesArray();
				int uvCoordIndicesArrayCount = uvCoordIndicesArray.getCount();
				for (int j = 0; j < uvCoordIndicesArrayCount; j++) {
					semantic = "TEXCOORD_" + std::to_string(j);
					buildAttributes[semantic] = std::vector<float>();
					semanticIndices[semantic] = uvCoordIndicesArray[j]->getIndices().getData();
					semanticData[semantic] = &colladaMesh->getUVCoords();
					primitive->attributes[semantic] = (GLTF::Accessor*)NULL;
				}
			}
			if (colladaPrimitive->hasColorIndices()) {
				COLLADAFW::IndexListArray& colorIndicesArray = colladaPrimitive->getColorIndicesArray();
				int colorIndicesArrayCount = colorIndicesArray.getCount();
				for (int j = 0; j < colorIndicesArrayCount; j++) {
					semantic = "COLOR_" + std::to_string(j);
					buildAttributes[semantic] = std::vector<float>();
					semanticIndices[semantic] = colorIndicesArray[j]->getIndices().getData();
					semanticData[semantic] = &colladaMesh->getColors();
					primitive->attributes[semantic] = (GLTF::Accessor*)NULL;
				}
			}
			int index = 0;
			int face = 0;
			int startFace = 0;
			int totalVertexCount = 0;
			int vertexCount = 0;
			int faceVertexCount = colladaPrimitive->getGroupedVerticesVertexCount(face);
			for (int j = 0; j < count; j++) {
				std::string attributeId;
				if (shouldTriangulate) {
					// This approach is very efficient in terms of runtime, but there are more correct solutions that may be worth considering.
					// Using a 3D variant of Fortune's Algorithm or something similar to compute a mesh with no overlapping triangles would be ideal.
					if (vertexCount >= faceVertexCount) {
						int end = buildIndices.size() - 1;
						if (faceVertexCount > 3) {
							// Make a triangle with the last two points and the first one
							buildIndices.push_back(buildIndices[end - 1]);
							buildIndices.push_back(buildIndices[end]);
							buildIndices.push_back(buildIndices[startFace]);
							totalVertexCount += 3;
						}
						face++;
						faceVertexCount = colladaPrimitive->getGroupedVerticesVertexCount(face);
						startFace = totalVertexCount;
						vertexCount = 0;
					}
					else if (vertexCount >= 3) {
						// Add the previous two points to complete the triangle
						int end = buildIndices.size() - 1;
						buildIndices.push_back(buildIndices[end - 1]);
						buildIndices.push_back(buildIndices[end]);
						totalVertexCount += 2;
					}
				}
				for (const auto& entry : semanticIndices) {
					semantic = entry.first;
					int numberOfComponents = 3;
					if (semantic.find("TEXCOORD") == 0) {
						numberOfComponents = 2;
					}
					attributeId += buildAttributeId(*semanticData[semantic], semanticIndices[semantic][j], numberOfComponents);
				}
				std::map<std::string, unsigned short>::iterator search = attributeIndicesMapping.find(attributeId);
				if (search != attributeIndicesMapping.end()) {
					buildIndices.push_back(search->second);
				}
				else {
					for (const auto& entry : buildAttributes) {
						semantic = entry.first;
						int numberOfComponents = 3;
						bool flipY = false;
						if (semantic.find("TEXCOORD") == 0) {
							numberOfComponents = 2;
							flipY = true;
						}
						int semanticIndex = semanticIndices[semantic][j];
						if (semantic == "POSITION") {
							mapping.push_back(semanticIndex);
						}
						const COLLADAFW::MeshVertexData* vertexData = semanticData[semantic];
						int stride = numberOfComponents;
						if (vertexData->getNumInputInfos() > 0) {
							stride = vertexData->getStride(0);
						}
						for (int k = 0; k < numberOfComponents; k++) {
							float value = getMeshVertexDataAtIndex(*vertexData, semanticIndex * stride + k);
							if (flipY && k == 1) {
								value = 1 - value;
							}
							buildAttributes[semantic].push_back(value);
						}
					}
					attributeIndicesMapping[attributeId] = index;
					buildIndices.push_back(index);
					index++;
				}
				totalVertexCount++;
				vertexCount++;
			}
			if (shouldTriangulate && faceVertexCount > 3) {
				// Close the last polyshape
				int end = buildIndices.size() - 1;
				buildIndices.push_back(buildIndices[end - 1]);
				buildIndices.push_back(buildIndices[end]);
				buildIndices.push_back(buildIndices[startFace]);
			}
			// Create indices accessor
			GLTF::Accessor* indices = new GLTF::Accessor(GLTF::Accessor::Type::SCALAR, GLTF::Constants::WebGL::UNSIGNED_SHORT, (unsigned char*)&buildIndices[0], buildIndices.size(), GLTF::Constants::WebGL::ELEMENT_ARRAY_BUFFER);
			primitive->indices = indices;
			mesh->primitives.push_back(primitive);
			// Create attribute accessors
			for (const auto& entry : buildAttributes) {
				std::string semantic = entry.first;
				std::vector<float> attributeData = entry.second;
				GLTF::Accessor::Type type = GLTF::Accessor::Type::VEC3;
				if (semantic.find("TEXCOORD") == 0) {
					type = GLTF::Accessor::Type::VEC2;
				}
				GLTF::Accessor* accessor = new GLTF::Accessor(type, GLTF::Constants::WebGL::FLOAT, (unsigned char*)&attributeData[0], attributeData.size() / GLTF::Accessor::getNumberOfComponents(type), GLTF::Constants::WebGL::ARRAY_BUFFER);
				primitive->attributes[semantic] = accessor;
			}
			positionMapping[primitive] = mapping;
		}
	}
	_meshMaterialPrimitiveMapping[uniqueId] = primitiveMaterialMapping;
	_meshPositionMapping[uniqueId] = positionMapping;
	_meshInstances[uniqueId] = mesh;
	return true;
}

bool COLLADA2GLTF::Writer::writeGeometry(const COLLADAFW::Geometry* geometry) {
	switch (geometry->getType()) {
	case COLLADAFW::Geometry::GEO_TYPE_MESH:
		if (!this->writeMesh((COLLADAFW::Mesh*)geometry)) {
			return false;
		}
		break;
	default:
		return false;
	}
	return true;
}

bool COLLADA2GLTF::Writer::writeMaterial(const COLLADAFW::Material* material) {
	this->_materialEffects[material->getUniqueId()] = material->getInstantiatedEffect();
	return true;
}

void packColladaColor(COLLADAFW::Color color, float* packArray) {
	packArray[0] = (float)color.getRed();
	packArray[1] = (float)color.getGreen();
	packArray[2] = (float)color.getBlue();
	packArray[3] = (float)color.getAlpha();
}

// Re-use this instance since the values don't change
GLTF::Sampler* globalSampler = new GLTF::Sampler();

GLTF::Texture* COLLADA2GLTF::Writer::fromColladaTexture(const COLLADAFW::EffectCommon* effectCommon, COLLADAFW::Texture colladaTexture) {
	GLTF::Texture* texture = new GLTF::Texture();
	const COLLADAFW::SamplerPointerArray& samplers = effectCommon->getSamplerPointerArray();
	COLLADAFW::Sampler* colladaSampler = (COLLADAFW::Sampler*)samplers[colladaTexture.getSamplerId()];
	GLTF::Image* image = _images[colladaSampler->getSourceImage()];
	texture->source = image;
	texture->sampler = globalSampler;
	return texture;
}

bool COLLADA2GLTF::Writer::writeEffect(const COLLADAFW::Effect* effect) {
	const COLLADAFW::CommonEffectPointerArray& commonEffects = effect->getCommonEffects();
	if (commonEffects.getCount() > 0) {
		GLTF::MaterialCommon* material = new GLTF::MaterialCommon();

		// One effect makes one template material, it really isn't possible to process more than one of these
		const COLLADAFW::EffectCommon* effectCommon = commonEffects[0];
		switch (effectCommon->getShaderType()) {
		case COLLADAFW::EffectCommon::SHADER_BLINN: 
			material->technique = GLTF::MaterialCommon::BLINN;
			break;
		case COLLADAFW::EffectCommon::SHADER_CONSTANT: 
			material->technique = GLTF::MaterialCommon::CONSTANT;
			break;
		case COLLADAFW::EffectCommon::SHADER_PHONG: 
			material->technique = GLTF::MaterialCommon::PHONG;
			break;
		case COLLADAFW::EffectCommon::SHADER_LAMBERT: 
			material->technique = GLTF::MaterialCommon::LAMBERT;
			break;
		}

		bool lockAmbientDiffuse = _extrasHandler->lockAmbientDiffuse.find(effect->getUniqueId()) != _extrasHandler->lockAmbientDiffuse.end();

		if (!lockAmbientDiffuse) {
			COLLADAFW::ColorOrTexture ambient = effectCommon->getAmbient();
			if (ambient.isTexture()) {
				material->values->ambientTexture = fromColladaTexture(effectCommon, ambient.getTexture());
			}
			else if (ambient.isColor()) {
				material->values->ambient = new float[4];
				packColladaColor(ambient.getColor(), material->values->ambient);
			}
		}

		COLLADAFW::ColorOrTexture diffuse = effectCommon->getDiffuse();
		if (diffuse.isTexture()) {
			material->values->diffuseTexture = fromColladaTexture(effectCommon, diffuse.getTexture());
			if (lockAmbientDiffuse) {
				material->values->ambientTexture = material->values->diffuseTexture;
			}
		}
		else if (diffuse.isColor()) {
			material->values->diffuse = new float[4];
			packColladaColor(diffuse.getColor(), material->values->diffuse);
			if (lockAmbientDiffuse) {
				material->values->ambient = material->values->diffuse;
			}
		}

		COLLADAFW::ColorOrTexture emission = effectCommon->getEmission();
		if (emission.isTexture()) {
			material->values->emissionTexture = fromColladaTexture(effectCommon, emission.getTexture());
		}
		else if (emission.isColor()) {
			material->values->emission = new float[4];
			packColladaColor(emission.getColor(), material->values->emission);
		}

		COLLADAFW::ColorOrTexture specular = effectCommon->getSpecular();
		if (specular.isTexture()) {
			material->values->specularTexture = fromColladaTexture(effectCommon, specular.getTexture());
		}
		else if (specular.isColor()) {
			material->values->specular = new float[4];
			packColladaColor(specular.getColor(), material->values->specular);
		}

		COLLADAFW::FloatOrParam shininess = effectCommon->getShininess();
		if (shininess.getType() == COLLADAFW::FloatOrParam::FLOAT) {
			float shininessValue = shininess.getFloatValue();
			if (shininessValue >= 0) {
				material->values->shininess = new float[1];
				material->values->shininess[0] = shininessValue;
			}
		}

		COLLADAFW::FloatOrParam transparency = effectCommon->getTransparency();
		if (transparency.getType() == COLLADAFW::FloatOrParam::FLOAT) {
			float transparencyValue = transparency.getFloatValue();
			if (transparencyValue >= 0) {
				material->values->transparency = new float[1];
				material->values->transparency[0] = transparencyValue;
			}
		}

		this->_effectInstances[effect->getUniqueId()] = material;
	}

	return true;
}

bool COLLADA2GLTF::Writer::writeCamera(const COLLADAFW::Camera* colladaCamera) {
	GLTF::Camera* writeCamera = NULL;
	if (colladaCamera->getCameraType() == COLLADAFW::Camera::ORTHOGRAPHIC) {
		GLTF::CameraOrthographic* camera = new GLTF::CameraOrthographic();
		camera->xmag = (float)colladaCamera->getXMag();
		camera->ymag = (float)colladaCamera->getYMag();
		writeCamera = camera;
	}
	else if (colladaCamera->getCameraType() == COLLADAFW::Camera::PERSPECTIVE) {
		GLTF::CameraPerspective* camera = new GLTF::CameraPerspective();
		float x = (float)(colladaCamera->getXFov().getValue() * (180.0 / 3.14));
		float y = (float)(colladaCamera->getYFov().getValue() * (180 / 3.14));
		float aspectRatio = (float)colladaCamera->getAspectRatio().getValue();
		switch (colladaCamera->getDescriptionType()) {
		case COLLADAFW::Camera::UNDEFINED:
		case COLLADAFW::Camera::SINGLE_X:
			camera->yfov = 1.0;
			break;
		case COLLADAFW::Camera::SINGLE_Y:
			camera->yfov = y;
			break;
		case COLLADAFW::Camera::X_AND_Y:
			camera->yfov = y;
			camera->aspectRatio = x / y;
			break;
		case COLLADAFW::Camera::ASPECTRATIO_AND_X:
			camera->yfov = x / aspectRatio;
			camera->aspectRatio = y;
			break;
		case COLLADAFW::Camera::ASPECTRATIO_AND_Y:
			camera->yfov = y;
			camera->aspectRatio = y;
			break;
		}
		writeCamera = camera;
	}
	if (writeCamera != NULL) {
		writeCamera->zfar = (float)colladaCamera->getFarClippingPlane().getValue();
		writeCamera->znear = (float)colladaCamera->getNearClippingPlane().getValue();
		_cameraInstances[colladaCamera->getUniqueId()] = writeCamera;
		return true;
	}
	return false;
}

bool COLLADA2GLTF::Writer::writeImage(const COLLADAFW::Image* colladaImage) {
	const COLLADABU::URI imageUri = colladaImage->getImageURI();
	path imagePath = path(_options->basePath) / imageUri.getURIString();
	std::string fileString = imagePath.string();
	std::string fileExtension = imagePath.extension().string();

	GLTF::Image* image;
	FILE* file = fopen(fileString.c_str(), "rb");
	if (file == NULL) {
		return false;
	}
	fseek(file, 0, SEEK_END);
	long int size = ftell(file);
	fclose(file);
	file = fopen(fileString.c_str(), "rb");
	unsigned char* buffer = (unsigned char*)malloc(size);
	int bytesRead = fread(buffer, sizeof(unsigned char), size, file);
	fclose(file);

	image = new GLTF::Image(imageUri.getPathFile(), buffer, bytesRead, fileExtension);

	_images[colladaImage->getUniqueId()] = image;
	return true;
}

bool COLLADA2GLTF::Writer::writeLight(const COLLADAFW::Light* colladaLight) {
	GLTF::MaterialCommon::Light* light = new GLTF::MaterialCommon::Light();
	switch (colladaLight->getLightType()) {
	case COLLADAFW::Light::AMBIENT_LIGHT:
		light->type = GLTF::MaterialCommon::Light::Type::AMBIENT;
		break;
	case COLLADAFW::Light::DIRECTIONAL_LIGHT:
		light->type = GLTF::MaterialCommon::Light::Type::DIRECTIONAL;
		break;
	case COLLADAFW::Light::POINT_LIGHT:
		light->type = GLTF::MaterialCommon::Light::Type::POINT;
		light->constantAttenuation = (float)colladaLight->getConstantAttenuation();
		light->linearAttenuation = (float)colladaLight->getLinearAttenuation();
		light->quadraticAttenuation = (float)colladaLight->getQuadraticAttenuation();
		break;
	}

	packColladaColor(colladaLight->getColor(), light->color);
	_lightInstances[colladaLight->getUniqueId()] = light;
	return true;
}

bool COLLADA2GLTF::Writer::writeAnimation(const COLLADAFW::Animation* animation) {
	GLTF::Animation::Sampler* sampler = new GLTF::Animation::Sampler();

	if (animation->getAnimationType() == COLLADAFW::Animation::ANIMATION_CURVE) {
		COLLADAFW::AnimationCurve *animationCurve = (COLLADAFW::AnimationCurve*)animation;
		COLLADAFW::FloatOrDoubleArray inputArray = animationCurve->getInputValues();
		COLLADAFW::FloatOrDoubleArray outputArray = animationCurve->getOutputValues();
		
		int inputLength = inputArray.getValuesCount();
		float* inputValues = new float[inputLength];
		int outputLength = outputArray.getValuesCount();
		float* outputValues = new float[outputLength];

		float value;
		for (int i = 0; i < inputLength; i++) {
			switch (inputArray.getType()) {
			case COLLADAFW::FloatOrDoubleArray::DATA_TYPE_DOUBLE:
				value = (float)(inputArray.getDoubleValues()->getData()[i]);
				break;
			case COLLADAFW::FloatOrDoubleArray::DATA_TYPE_FLOAT:
				value = inputArray.getFloatValues()->getData()[i];
				break;
			}
			inputValues[i] = value;
		}
		for (int i = 0; i < outputLength; i++) {
			switch (outputArray.getType()) {
			case COLLADAFW::FloatOrDoubleArray::DATA_TYPE_DOUBLE:
				value = (float)(outputArray.getDoubleValues()->getData()[i]);
				break;
			case COLLADAFW::FloatOrDoubleArray::DATA_TYPE_FLOAT:
				value = outputArray.getFloatValues()->getData()[i];
				break;
			}
			outputValues[i] = value;
		}

		GLTF::Accessor* inputAccessor = new GLTF::Accessor(GLTF::Accessor::Type::SCALAR, GLTF::Constants::WebGL::FLOAT, (unsigned char*)inputValues, inputLength, (GLTF::Constants::WebGL)-1);
		// The type is unknown at this point; this may get replaced during writeAnimationList
		GLTF::Accessor* outputAccessor = new GLTF::Accessor(GLTF::Accessor::Type::SCALAR, GLTF::Constants::WebGL::FLOAT, (unsigned char*)outputValues, outputLength, (GLTF::Constants::WebGL)-1);
		
		sampler->input = inputAccessor;
		sampler->output = outputAccessor;
		
		_animationSamplers[animation->getUniqueId()] = sampler;
	}
	return true;
}

bool COLLADA2GLTF::Writer::writeAnimationList(const COLLADAFW::AnimationList* animationList) {
	const COLLADAFW::AnimationList::AnimationBindings& bindings = animationList->getAnimationBindings();
	GLTF::Node* node = _animatedNodes[animationList->getUniqueId()];
	GLTF::Animation* animation = new GLTF::Animation();

	double* component = new double[1];
	for (size_t i = 0; i < bindings.getCount(); i++) {
		const COLLADAFW::AnimationList::AnimationBinding& binding = bindings[i];
		GLTF::Animation::Channel* channel = new GLTF::Animation::Channel();
		GLTF::Animation::Channel::Target* target = new GLTF::Animation::Channel::Target();
		channel->target = target;
		animation->channels.push_back(channel);
		target->node = node;
		GLTF::Animation::Sampler* sampler = _animationSamplers[binding.animation];
		channel->sampler = sampler;
		GLTF::Accessor* output = sampler->output;

		int usesIndex = -1;
		float* outputData = NULL;
		GLTF::Accessor::Type type;
		int count = output->count;

		GLTF::Node::Transform* transform = node->transform;
		if (transform->type == GLTF::Node::Transform::MATRIX) {
			transform = ((GLTF::Node::TransformMatrix*)transform)->getTransformTRS();
			node->transform = transform;
		}

		switch (binding.animationClass) {
		case COLLADAFW::AnimationList::AnimationClass::MATRIX4X4: {
			float* translation = new float[count / 16 * 3];
			float* rotation = new float[count / 16 * 4];
			float* scale = new float[count / 16 * 3];
			GLTF::Node::TransformMatrix* transformMatrix = new GLTF::Node::TransformMatrix();
			GLTF::Node::TransformTRS* transformTRS = new GLTF::Node::TransformTRS();
			float* lastRotation = NULL;
			for (int j = 0; j < count / 16; j++) {
				for (int m = 0; m < 4; m++) {
					for (int n = 0; n < 4; n++) {
						output->getComponentAtIndex(j * 16 + m * 4 + n, component);
						transformMatrix->matrix[n * 4 + m] = (float)component[0];
					}
				}
				transformMatrix->getTransformTRS(transformTRS);
				for (int k = 0; k < 3; k++) {
					translation[j * 3 + k] = transformTRS->translation[k];
				}
				// Sometimes when we decompose a matrix, the quaternion flips; we want to make sure that the quaternion distance between frames is minimal
				if (lastRotation != NULL) {
					float flippedDifference = 0.0;
					float difference = 0.0;
					for (int k = 0; k < 4; k++) {
						flippedDifference += (-transformTRS->rotation[k] - lastRotation[k]) * (-transformTRS->rotation[k] - lastRotation[k]);
						difference += (transformTRS->rotation[k] - lastRotation[k]) * (transformTRS->rotation[k] - lastRotation[k]);
					}
					if (flippedDifference < difference) {
						// Flip the quaternion
						for (int k = 0; k < 4; k++) {
							transformTRS->rotation[k] = -transformTRS->rotation[k];
						}
					}
				}
				else {
					lastRotation = new float[4];
				}
				for (int k = 0; k < 4; k++) {
					rotation[j * 4 + k] = transformTRS->rotation[k];
					lastRotation[k] = transformTRS->rotation[k];
				}
				for (int k = 0; k < 3; k++) {
					scale[j * 3 + k] = transformTRS->scale[k];
				}
			}
			animation->channels.clear();
			GLTF::Animation::Channel* translationChannel = new GLTF::Animation::Channel();
			GLTF::Animation::Channel::Target* translationTarget = new GLTF::Animation::Channel::Target();
			translationTarget->path = GLTF::Animation::Channel::Target::Path::TRANSLATION;
			translationTarget->node = node;
			translationChannel->target = translationTarget;
			animation->channels.push_back(translationChannel);
			GLTF::Animation::Sampler* translationSampler = new GLTF::Animation::Sampler();
			translationChannel->sampler = translationSampler;
			translationSampler->input = sampler->input;
			translationSampler->output = new GLTF::Accessor(GLTF::Accessor::Type::VEC3, GLTF::Constants::WebGL::FLOAT, (unsigned char*)translation, count / 16, (GLTF::Constants::WebGL)-1);

			GLTF::Animation::Channel* rotationChannel = new GLTF::Animation::Channel();
			GLTF::Animation::Channel::Target* rotationTarget = new GLTF::Animation::Channel::Target();
			rotationTarget->path = GLTF::Animation::Channel::Target::Path::ROTATION;
			rotationTarget->node = node;
			rotationChannel->target = rotationTarget;
			animation->channels.push_back(rotationChannel);
			GLTF::Animation::Sampler* rotationSampler = new GLTF::Animation::Sampler();
			rotationChannel->sampler = rotationSampler;
			rotationSampler->input = sampler->input;
			rotationSampler->output = new GLTF::Accessor(GLTF::Accessor::Type::VEC4, GLTF::Constants::WebGL::FLOAT, (unsigned char*)rotation, count / 16, (GLTF::Constants::WebGL)-1);

			GLTF::Animation::Channel* scaleChannel = new GLTF::Animation::Channel();
			GLTF::Animation::Channel::Target* scaleTarget = new GLTF::Animation::Channel::Target();
			scaleTarget->path = GLTF::Animation::Channel::Target::Path::SCALE;
			scaleTarget->node = node;
			scaleChannel->target = scaleTarget;
			animation->channels.push_back(scaleChannel);
			GLTF::Animation::Sampler* scaleSampler = new GLTF::Animation::Sampler();
			scaleChannel->sampler = scaleSampler;
			scaleSampler->input = sampler->input;
			scaleSampler->output = new GLTF::Accessor(GLTF::Accessor::Type::VEC3, GLTF::Constants::WebGL::FLOAT, (unsigned char*)scale, count / 16, (GLTF::Constants::WebGL)-1);
			break;
		}
		case COLLADAFW::AnimationList::AnimationClass::POSITION_XYZ: {
			// The output data is already in the correct format
			outputData = (float*)output->bufferView->buffer->data;
			type = GLTF::Accessor::Type::VEC3;
			target->path = GLTF::Animation::Channel::Target::Path::TRANSLATION;
			output = new GLTF::Accessor(type, GLTF::Constants::WebGL::FLOAT, (unsigned char*)outputData, count / 3, (GLTF::Constants::WebGL)-1);
			sampler->output = output;
			break;
		}
		case COLLADAFW::AnimationList::AnimationClass::POSITION_X: {
			usesIndex = 0;
		}
		case COLLADAFW::AnimationList::AnimationClass::POSITION_Y: {
			if (usesIndex < 0) {
				usesIndex = 1;
			}
		}
		case COLLADAFW::AnimationList::AnimationClass::POSITION_Z: {
			if (usesIndex < 0) {
				usesIndex = 2;
			}
			// The output data needs to be padded with 0's 
			float* outputData = new float[count * 3];
			type = GLTF::Accessor::Type::VEC3;
			target->path = GLTF::Animation::Channel::Target::Path::TRANSLATION;
			for (int j = 0; j < count; j++) {
				output->getComponentAtIndex(j, component);
				for (int k = 0; k < 3; k++) {
					if (k == usesIndex) {
						outputData[j * 3 + k] = (float)component[0];
					}
					else {
						outputData[j * 3 + k] = 0;
					}
				}
			}
			output = new GLTF::Accessor(type, GLTF::Constants::WebGL::FLOAT, (unsigned char*)outputData, count, (GLTF::Constants::WebGL)-1);
			sampler->output = output;
			break;
		}
		case COLLADAFW::AnimationList::AXISANGLE: {
			// The output data is already in the correct format
			outputData = (float*)output->bufferView->buffer->data;
			type = GLTF::Accessor::Type::VEC4;
			target->path = GLTF::Animation::Channel::Target::Path::ROTATION;
			output = new GLTF::Accessor(type, GLTF::Constants::WebGL::FLOAT, (unsigned char*)outputData, count / 4, (GLTF::Constants::WebGL)-1);
			sampler->output = output;
			break;
		}
		case COLLADAFW::AnimationList::ANGLE: {
			// The angle needs to be applied to the existing quaternion
			if (transform != NULL) {
				if (transform->type == GLTF::Node::Transform::TRS) {
					GLTF::Node::TransformTRS* transformTRS = (GLTF::Node::TransformTRS*)transform;
					float* rotation = transformTRS->rotation;
					outputData = new float[count * 4];
					type = GLTF::Accessor::Type::VEC4;
					target->path = GLTF::Animation::Channel::Target::Path::ROTATION;

					COLLADABU::Math::Real angle;
					COLLADABU::Math::Vector3 axis;
					for (int j = 0; j < count; j++) {
						COLLADABU::Math::Quaternion quaternion = COLLADABU::Math::Quaternion(rotation[3], rotation[0], rotation[1], rotation[2]);
						quaternion.toAngleAxis(angle, axis);
						output->getComponentAtIndex(j, component);
						angle = COLLADABU::Math::Utils::degToRad(component[0]);
						quaternion.fromAngleAxis(angle, axis);
						outputData[j * 4] = (float)quaternion.x;
						outputData[j * 4 + 1] = (float)quaternion.y;
						outputData[j * 4 + 2] = (float)quaternion.z;
						outputData[j * 4 + 3] = (float)quaternion.w;
					}
				}
			}
			output = new GLTF::Accessor(type, GLTF::Constants::WebGL::FLOAT, (unsigned char*)outputData, count, (GLTF::Constants::WebGL)-1);
			sampler->output = output;
			break;
		}}
	}
	_asset->animations.push_back(animation);
	return true;
}

bool COLLADA2GLTF::Writer::writeSkinControllerData(const COLLADAFW::SkinControllerData* skinControllerData) {
	GLTF::Skin* skin = new GLTF::Skin();
	COLLADAFW::UniqueId uniqueId = skinControllerData->getUniqueId();
	skin->name = skinControllerData->getName();

	// Write inverseBindMatrices and bindShapeMatrix
	const COLLADAFW::Matrix4Array& matrixArray = skinControllerData->getInverseBindMatrices();
	size_t matrixArrayCount = matrixArray.getCount();
	float* inverseBindMatrices = new float[matrixArrayCount * 16];
	for (size_t i = 0; i < matrixArrayCount; i++) {
		packColladaMatrix(matrixArray[i], inverseBindMatrices, i * 16);
	}
	skin->inverseBindMatrices = new GLTF::Accessor(GLTF::Accessor::Type::MAT4, GLTF::Constants::WebGL::FLOAT, (unsigned char*)inverseBindMatrices, matrixArrayCount, (GLTF::Constants::WebGL)-1);
	packColladaMatrix(skinControllerData->getBindShapeMatrix(), skin->bindShapeMatrix, 0);

	// Cache joint and weight data
	// COLLADA can have different numbers of joints for a single vertex
	// We have to make this uniform across all vertices to make it into a GLTF primitive attribute
	size_t maxJointsPerVertex = 0;
	const COLLADAFW::UIntValuesArray& jointsPerVertexArray = skinControllerData->getJointsPerVertex();
	
	size_t totalVertices = 0;
	for (size_t i = 0; i < jointsPerVertexArray.getCount(); i++) {
		size_t jointsPerVertex = jointsPerVertexArray[i];
		totalVertices += jointsPerVertex;
		if (jointsPerVertex > maxJointsPerVertex) {
			maxJointsPerVertex = jointsPerVertex;
		}
	}

	// Right now most loaders require JOINT and WEIGHT to be `vec4`
	maxJointsPerVertex = 4;

	GLTF::Accessor::Type type;
	if (maxJointsPerVertex == 1) {
		type = GLTF::Accessor::Type::SCALAR;
	} else if (maxJointsPerVertex == 2) {
		type = GLTF::Accessor::Type::VEC2;
	} else if (maxJointsPerVertex == 3) {
		type = GLTF::Accessor::Type::VEC3;
	} else if (maxJointsPerVertex == 4) {
		type = GLTF::Accessor::Type::VEC4;
	} else if (maxJointsPerVertex <= 9) {
		type = GLTF::Accessor::Type::MAT3;
	} else if (maxJointsPerVertex <= 16) {
		type = GLTF::Accessor::Type::MAT4;
	} else {
		// There is no GLTF accessor type big enough to store this many joint influences
		return false;
	}
	maxJointsPerVertex = GLTF::Accessor::getNumberOfComponents(type);

	size_t vertexCount = skinControllerData->getVertexCount();
	size_t offset = 0;
	const COLLADAFW::IntValuesArray& jointIndicesArray = skinControllerData->getJointIndices();
	const COLLADAFW::UIntValuesArray& weightIndicesArray = skinControllerData->getWeightIndices();
	const COLLADAFW::FloatOrDoubleArray& weightsArray = skinControllerData->getWeights();

	std::vector<int*> joints;
	std::vector<float*> weights;
	for (size_t i = 0; i < vertexCount; i++) {
		unsigned int jointsPerVertex = jointsPerVertexArray[i];
		int* joint = new int[maxJointsPerVertex];
		float* weight = new float[maxJointsPerVertex];
		for (size_t j = 0; j < maxJointsPerVertex; j++) {
			if (j < jointsPerVertex) {
				unsigned int jointIndex = jointIndicesArray[j + offset];
				joint[j] = jointIndex;
				unsigned int weightIndex = weightIndicesArray[j + offset];
				float weightValue;
				if (weightsArray.getType() == COLLADAFW::FloatOrDoubleArray::DATA_TYPE_FLOAT) {
					weightValue = weightsArray.getFloatValues()->getData()[weightIndex];
				} else if (weightsArray.getType() == COLLADAFW::FloatOrDoubleArray::DATA_TYPE_DOUBLE) {
					weightValue = (float)weightsArray.getDoubleValues()->getData()[weightIndex];
				}
				weight[j] = weightValue;
			} else {
				joint[j] = 0;
				weight[j] = 0;
			}
		}
		offset += jointsPerVertex;
		joints.push_back(joint);
		weights.push_back(weight);
	}
	_skinData[uniqueId] = std::tuple<GLTF::Accessor::Type, std::vector<int*>, std::vector<float*>>(type, joints, weights);
	_skinInstances[uniqueId] = skin;
	return true;
}

/**
* Creates a placeholder <GLTF::Skin> for each <COLLADAFW::SkinController>.
* The produced skins are stored in `_skinInstances` indexed by their <COLLADAFW::UniqueId>.
*
* This is expected to run before nodes are written, so the targeted joint nodes are stored
* in a set of <COLLADAFW::UniqueId> for each SkinController id on _skinJointNodes. When nodes
* are written, this is used to assign <GLTF::Node> references for joints.
*
* @param controller The COLLADA skin controller to write to glTF
* @return `true` if the operation completed succesfully, `false` if an error occured
*/
bool COLLADA2GLTF::Writer::writeController(const COLLADAFW::Controller* controller) {
	if (controller->getControllerType() == COLLADAFW::Controller::CONTROLLER_TYPE_SKIN) {
		COLLADAFW::SkinController* skinController = (COLLADAFW::SkinController*)controller;
		COLLADAFW::UniqueId skinControllerDataId = skinController->getSkinControllerData();
		COLLADAFW::UniqueId skinControllerId = skinController->getUniqueId();
		GLTF::Skin* skin = _skinInstances[skinControllerDataId];
		COLLADAFW::UniqueIdArray& jointIds = skinController->getJoints();
		for (size_t i = 0; i < jointIds.getCount(); i++) {
			_skinJointNodes[skinControllerId].push_back(jointIds[i]);
		}
		GLTF::Accessor::Type type;
		std::vector<int*> joints;
		std::vector<float*> weights;
		std::tie(type, joints, weights) = _skinData[skinControllerDataId];
		int numberOfComponents = GLTF::Accessor::getNumberOfComponents(type);

		COLLADAFW::UniqueId meshId = skinController->getSource();
		GLTF::Mesh* mesh = _meshInstances[meshId];

		double* jointComponent = new double[numberOfComponents];
		double* weightComponent = new double[numberOfComponents];
		std::map<GLTF::Primitive*, std::vector<int>> positionMapping = _meshPositionMapping[meshId];
		for (const auto& primitiveEntry : positionMapping) {
			GLTF::Primitive* primitive = primitiveEntry.first;
			int count = primitive->attributes["POSITION"]->count;
			unsigned short* jointArray = new unsigned short[count * numberOfComponents];
			float* weightArray = new float[count * numberOfComponents];

			std::vector<int> mapping = primitiveEntry.second;
			for (int i = 0; i < count; i++) {
				int index = mapping[i];
				int* joint = joints[index];
				float* weight = weights[index];
				for (int j = 0; j < numberOfComponents; j++) {
					jointArray[i * numberOfComponents + j] = joint[j];
					weightArray[i * numberOfComponents + j] = weight[j];
				}
			}
			GLTF::Accessor* weightAccessor = new GLTF::Accessor(type, GLTF::Constants::WebGL::FLOAT, (unsigned char*)weightArray, count, GLTF::Constants::WebGL::ARRAY_BUFFER);
			primitive->attributes["WEIGHT"] = weightAccessor;
			GLTF::Accessor* jointAccessor = new GLTF::Accessor(type, GLTF::Constants::WebGL::UNSIGNED_SHORT, (unsigned char*)jointArray, count, GLTF::Constants::WebGL::ARRAY_BUFFER);
			primitive->attributes["JOINT"] = jointAccessor;
		}

		_skinInstances[skinControllerId] = skin;
		_skinnedMeshes[skinControllerId] = mesh;
	}
	return true;
}

bool COLLADA2GLTF::Writer::writeFormulas(const COLLADAFW::Formulas* formulas) {
	return true;
}

bool COLLADA2GLTF::Writer::writeKinematicsScene(const COLLADAFW::KinematicsScene* kinematicsScene) {
	return true;
}
