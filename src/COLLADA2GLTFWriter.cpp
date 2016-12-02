#include "COLLADA2GLTFWriter.h"

COLLADA2GLTF::Writer::Writer(GLTF::Asset * asset) : _asset(asset) {
	_indicesBufferView = new GLTF::BufferView(NULL, 0, GLTF::Constants::WebGL::ELEMENT_ARRAY_BUFFER);
	_attributesBufferView = new GLTF::BufferView(NULL, 0, GLTF::Constants::WebGL::ARRAY_BUFFER);
}

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
	GLTF::Node* node = new GLTF::Node();
	node->id = colladaNode->getOriginalId();
	node->name = colladaNode->getName();

	// Instance Geometries
	const COLLADAFW::InstanceGeometryPointerArray& instanceGeometries = colladaNode->getInstanceGeometries();
	int count = instanceGeometries.getCount();
	if (count > 0) {
		for (int i = 0; i < count; i++) {
			COLLADAFW::InstanceGeometry* instanceGeometry = instanceGeometries[i];
			COLLADAFW::MaterialBindingArray& materialBindings = instanceGeometry->getMaterialBindings();
			const COLLADAFW::UniqueId& objectId = instanceGeometry->getUniqueId();
			std::map<COLLADAFW::UniqueId, GLTF::Mesh*>::iterator iter = _meshInstances.find(objectId);
			if (iter != _meshInstances.end()) {
				GLTF::Mesh* mesh = iter->second;
				int materialBindingsCount = materialBindings.getCount();
				if (materialBindingsCount > 0) {
					for (int j = 0; j < materialBindingsCount; j++) {
						COLLADAFW::MaterialBinding materialBinding = materialBindings[j];
						GLTF::Primitive* primitive = mesh->primitives[j];
						GLTF::Material* material = _materialInstances[materialBinding.getReferencedMaterial()];
						if (primitive->material != NULL && primitive->material != material) {
							// This mesh primitive has a different material from a previous instance, clone the mesh and primitives
							// TODO: add clone to GLTF::Object
						}
						primitive->material = material;
					}
				}
				node->meshes.push_back(mesh);
			}
		}
	}

	// Add to the group and recurse child nodes
	group->push_back(node);
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
	for (int i = 0; i < count; i++) {
		unsigned int rootIndex = rootIndices[i];
		unsigned int index = indices[i];
		if (rootIndex != index) {
			indicesMapping->at(semantic)[index] = rootIndex;
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

/**
 * Converts and writes a <COLLADAFW::Mesh> to a <GLTF::Mesh>.
 * The produced meshes are stored in `this->_meshInstances` indexed by their <COLLADAFW::UniqueId>.
 * 
 * COLLADA has different sets of indices per-primitive for each attribute, and glTF only has a single
 * set of indices for a primitive. The indices for POSITION are used, and any other attributes
 * are re-mapped to align with those indices in the glTF.
 *
 * Currently, the <COLLADAFW::MeshPrimitive::PrimitiveType> types `POLYGONS`, and `POLYLIST` are not supported
 * since they have no direct analogue in glTF. Primitives with this type will be skipped.
 *
 * @param colladaMesh The COLLADA mesh to write to glTF
 * @return `true` if the operation completed succesfully, `false` if an error occured
 */
bool COLLADA2GLTF::Writer::writeMesh(const COLLADAFW::Mesh* colladaMesh) {
	GLTF::Mesh* mesh = new GLTF::Mesh();
	mesh->id = colladaMesh->getOriginalId();
	mesh->name = colladaMesh->getName();

	const COLLADAFW::MeshPrimitiveArray& meshPrimitives = colladaMesh->getMeshPrimitives();
	int meshPrimitivesCount = meshPrimitives.getCount();
	std::map<std::string, std::map<int, int>> indicesMapping;
	if (meshPrimitivesCount > 0) {
		// Create primitive indices accessor and mappings
		for (int i = 0; i < meshPrimitivesCount; i++) {
			COLLADAFW::MeshPrimitive* colladaPrimitive = meshPrimitives[i];
			GLTF::Primitive* primitive = new GLTF::Primitive();
			COLLADAFW::MeshPrimitive::PrimitiveType type = colladaPrimitive->getPrimitiveType();
			switch (colladaPrimitive->getPrimitiveType()) {
			case COLLADAFW::MeshPrimitive::LINES:
				primitive->mode = GLTF::Primitive::Mode::LINES;
				break;
			case COLLADAFW::MeshPrimitive::LINE_STRIPS:
				primitive->mode = GLTF::Primitive::Mode::LINE_STRIP;
				break;
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
			}
			if (primitive->mode == GLTF::Primitive::Mode::UNKNOWN) {
				continue;
			}
			int count = colladaPrimitive->getPositionIndices().getCount();
			const unsigned int* indices = colladaPrimitive->getPositionIndices().getData();
			GLTF::Accessor* indicesAccessor = new GLTF::Accessor(GLTF::Accessor::Type::SCALAR, GLTF::Constants::WebGL::UNSIGNED_SHORT, (unsigned char*)indices, count, this->_indicesBufferView);
			primitive->indices = indicesAccessor;

			std::map<std::string, GLTF::Accessor*>* attributes = new std::map<std::string, GLTF::Accessor*>();
			attributes->emplace("POSITION", (GLTF::Accessor*)NULL);
			if (colladaPrimitive->hasNormalIndices()) {
				mapAttributeIndices(indices, colladaPrimitive->getNormalIndices().getData(), count, "NORMAL", &primitive->attributes, &indicesMapping);
			}
			if (colladaPrimitive->hasBinormalIndices()) {
				mapAttributeIndices(indices, colladaPrimitive->getBinormalIndices().getData(), count, "BINORMAL", &primitive->attributes, &indicesMapping);
			}
			if (colladaPrimitive->hasTangentIndices()) {
				mapAttributeIndices(indices, colladaPrimitive->getTangentIndices().getData(), count, "TANGENT", &primitive->attributes, &indicesMapping);
			}
			if (colladaPrimitive->hasUVCoordIndices()) {
				mapAttributeIndicesArray(indices, colladaPrimitive->getUVCoordIndicesArray(), count, "TEXCOORD", &primitive->attributes, &indicesMapping);
			}
			if (colladaPrimitive->hasColorIndices()) {
				mapAttributeIndicesArray(indices, colladaPrimitive->getColorIndicesArray(), count, "COLOR", &primitive->attributes, &indicesMapping);
			}
			mesh->primitives.push_back(primitive);
		}
		// Create mesh attribute accessors and use the indices mapping to move attributes as necessary to unify the indices
		std::map<std::string, GLTF::Accessor*> meshAttributes;
		for (GLTF::Primitive* primitive : mesh->primitives) {
			for (auto const& attribute : primitive->attributes) {
				std::string semantic = attribute.first;
				if (meshAttributes.find(semantic) == meshAttributes.end()) {
					if (semantic == "POSITION") {
						meshAttributes[semantic] = bufferAndMapVertexData(this->_attributesBufferView, GLTF::Accessor::Type::VEC3, colladaMesh->getPositions(), indicesMapping[semantic]);
					}
					else if (semantic == "NORMAL") {
						meshAttributes[semantic] = bufferAndMapVertexData(this->_attributesBufferView, GLTF::Accessor::Type::VEC3, colladaMesh->getNormals(), indicesMapping[semantic]);
					}
					else if (semantic == "TANGENT") {
						meshAttributes[semantic] = bufferAndMapVertexData(this->_attributesBufferView, GLTF::Accessor::Type::VEC3, colladaMesh->getTangents(), indicesMapping[semantic]);
					}
					else if (semantic.find("TEXCOORD") == 0) {
						meshAttributes[semantic] = bufferAndMapVertexData(this->_attributesBufferView, GLTF::Accessor::Type::VEC2, colladaMesh->getUVCoords(), indicesMapping[semantic]);
					}
					else if (semantic.find("COLOR") == 0) {
						meshAttributes[semantic] = bufferAndMapVertexData(this->_attributesBufferView, GLTF::Accessor::Type::VEC3, colladaMesh->getColors(), indicesMapping[semantic]);
					}
				}
				if (meshAttributes[semantic] == NULL) {
					return false;
				}
				primitive->attributes.emplace(semantic, meshAttributes[semantic]);
			}
		}
	}
	this->_meshInstances[colladaMesh->getUniqueId()] = mesh;
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
