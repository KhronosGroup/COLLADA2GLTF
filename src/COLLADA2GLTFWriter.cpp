#include "COLLADA2GLTFWriter.h"

COLLADA2GLTF::Writer::Writer(GLTF::Asset* asset, COLLADA2GLTF::Options* options) : _asset(asset) {
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
			const COLLADAFW::UniqueId& objectId = instanceGeometry->getInstanciatedObjectId();
			std::map<COLLADAFW::UniqueId, GLTF::Mesh*>::iterator iter = _meshInstances.find(objectId);
			if (iter != _meshInstances.end()) {
				GLTF::Mesh* mesh = iter->second;
				int materialBindingsCount = materialBindings.getCount();
				if (materialBindingsCount > 0) {
					for (int j = 0; j < materialBindingsCount; j++) {
						COLLADAFW::MaterialBinding materialBinding = materialBindings[j];
						GLTF::Primitive* primitive = mesh->primitives[j];
						COLLADAFW::UniqueId materialId = materialBinding.getReferencedMaterial();
						COLLADAFW::UniqueId effectId = this->_materialEffects[materialId];
						GLTF::Material* material = _effectInstances[effectId];
						material->id = materialBinding.getName();
						if (primitive->material != NULL && primitive->material != material) {
							// This mesh primitive has a different material from a previous instance, clone the mesh and primitives
							mesh = (GLTF::Mesh*)mesh->clone();
							primitive = mesh->primitives[j];
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
		id += std::to_string(getMeshVertexDataAtIndex(data, index + i)) + ":";
	}
	return id;
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
	std::map<std::string, std::vector<float>> buildAttributes;
	std::map<std::string, unsigned short> attributeIndicesMapping;
	if (meshPrimitivesCount > 0) {
		// Create primitives
		for (int i = 0; i < meshPrimitivesCount; i++) {
			std::vector<unsigned short> buildIndices;
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
			for (int j = 0; j < count; j++) {
				std::string attributeId;
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
						if (semantic.find("TEXCOORD") == 0) {
							numberOfComponents = 2;
						}
						for (int k = 0; k < numberOfComponents; k++) {
							buildAttributes[semantic].push_back(getMeshVertexDataAtIndex(*semanticData[semantic], semanticIndices[semantic][j] * numberOfComponents + k));
						}
					}
					attributeIndicesMapping[attributeId] = index;
					buildIndices.push_back(index);
					index++;
				}
			}
			// Create indices accessor
			primitive->indices = new GLTF::Accessor(GLTF::Accessor::Type::SCALAR, GLTF::Constants::WebGL::UNSIGNED_SHORT, (unsigned char*)&buildIndices[0], buildIndices.size(), this->_indicesBufferView);
			mesh->primitives.push_back(primitive);
		}
		// Create attribute accessors
		for (const auto& entry : buildAttributes) {
			std::string semantic = entry.first;
			std::vector<float> attributeData = entry.second;
			GLTF::Accessor::Type type = GLTF::Accessor::Type::VEC3;
			if (semantic.find("TEXCOORD") == 0) {
				type = GLTF::Accessor::Type::VEC2;
			}
			GLTF::Accessor* accessor = new GLTF::Accessor(type, GLTF::Constants::WebGL::FLOAT, (unsigned char*)&attributeData[0], attributeData.size() / GLTF::Accessor::getNumberOfComponents(type), this->_attributesBufferView);
			for (GLTF::Primitive* primitive : mesh->primitives) {
				std::map<std::string, GLTF::Accessor*>::iterator search = primitive->attributes.find(semantic);
				if (search != primitive->attributes.end()) {
					// Only apply if this primitive uses this particular semantic
					primitive->attributes[semantic] = accessor;
				}
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
	this->_materialEffects[material->getUniqueId()] = material->getInstantiatedEffect();
	return true;
}

void packColladaColor(COLLADAFW::Color color, float* packArray) {
	packArray[0] = color.getRed();
	packArray[1] = color.getGreen();
	packArray[2] = color.getBlue();
	packArray[3] = color.getAlpha();
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

		COLLADAFW::ColorOrTexture ambient = effectCommon->getAmbient();
		if (ambient.isColor()) {
			packColladaColor(ambient.getColor(), material->values->ambient);
		}

		COLLADAFW::ColorOrTexture diffuse = effectCommon->getDiffuse();
		if (diffuse.isTexture()) {
			// material->values->diffuseTexture = diffuse.getTexture();
		}
		else if (diffuse.isColor()) {
			packColladaColor(diffuse.getColor(), material->values->diffuse);
		}

		COLLADAFW::ColorOrTexture emission = effectCommon->getEmission();
		if (emission.isColor()) {
			packColladaColor(emission.getColor(), material->values->emission);
		}

		COLLADAFW::ColorOrTexture specular = effectCommon->getSpecular();
		if (specular.isColor()) {
			packColladaColor(specular.getColor(), material->values->specular);
		}

		COLLADAFW::FloatOrParam shininess = effectCommon->getShininess();
		if (shininess.getType() == COLLADAFW::FloatOrParam::FLOAT) {
			material->values->shininess[0] = shininess.getFloatValue();
		}

		this->_effectInstances[effect->getUniqueId()] = material;
	}

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
