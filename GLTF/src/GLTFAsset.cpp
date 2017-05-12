#include "GLTFAsset.h"

#include <algorithm>
#include <functional>
#include <map>
#include <set>

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

std::map<GLTF::Image*, GLTF::Texture*> _pbrTextureCache;

GLTF::Asset::Asset() {
	metadata = new GLTF::Asset::Metadata();
	globalSampler = new GLTF::Sampler();
}

void GLTF::Asset::Metadata::writeJSON(void* writer, GLTF::Options* options) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;
	if (copyright.length() > 0) {
		jsonWriter->Key("copyright");
		jsonWriter->String(copyright.c_str());
	}
	if (generator.length() > 0) {
		jsonWriter->Key("generator");
		jsonWriter->String(generator.c_str());
	}
	if (version.length() > 0) {
		jsonWriter->Key("version");
		jsonWriter->String(version.c_str());
	}
	GLTF::Object::writeJSON(writer, options);
}

GLTF::Scene* GLTF::Asset::getDefaultScene() {
	GLTF::Scene* scene;
	if (this->scene < 0) {
		scene = new GLTF::Scene();
		this->scenes.push_back(scene);
		this->scene = 0;
	}
	else {
		scene = this->scenes[this->scene];
	}
	return scene;
}

std::set<GLTF::Accessor*> GLTF::Asset::getAllAccessors() {
	std::set<GLTF::Accessor*> accessors;
	for (GLTF::Skin* skin : getAllSkins()) {
		GLTF::Accessor* inverseBindMatrices = skin->inverseBindMatrices;
		if (inverseBindMatrices != NULL) {
			accessors.insert(inverseBindMatrices);
		}
	}

	for (GLTF::Primitive* primitive : getAllPrimitives()) {
		for (const auto attribute : primitive->attributes) {
			accessors.insert(attribute.second);
		}
		GLTF::Accessor* indicesAccessor = primitive->indices;
		if (indicesAccessor != NULL) {
			accessors.insert(indicesAccessor);
		}
	}

	for (GLTF::Animation* animation : animations) {
		for (GLTF::Animation::Channel* channel : animation->channels) {
			GLTF::Animation::Sampler* sampler = channel->sampler;
			accessors.insert(sampler->input);
			accessors.insert(sampler->output);
		}
	}
	return accessors;
}

std::set<GLTF::Node*> GLTF::Asset::getAllNodes() {
	std::vector<GLTF::Node*> nodeStack;
	std::set<GLTF::Node*> nodes;
	for (GLTF::Node* node : getDefaultScene()->nodes) {
		nodeStack.push_back(node);
	}
	while (nodeStack.size() > 0) {
		GLTF::Node* node = nodeStack.back();
		std::set<GLTF::Node*>::iterator it = std::find(nodes.begin(), nodes.end(), node);
		if (it == nodes.end()) {
			nodes.insert(node);
		}
		nodeStack.pop_back();
		for (GLTF::Node* child : node->children) {
			nodeStack.push_back(child);
		}
		GLTF::Skin* skin = node->skin;
		if (skin != NULL) {
			GLTF::Node* skeleton = skin->skeleton;
			if (skeleton != NULL) {
				nodeStack.push_back(skeleton);
			}
			for (GLTF::Node* jointNode : skin->joints) {
				nodeStack.push_back(jointNode);
			}
		}
	}
	return nodes;
}

std::set<GLTF::Mesh*> GLTF::Asset::getAllMeshes() {
	std::set<GLTF::Mesh*> meshes;
	for (GLTF::Node* node : getAllNodes()) {
		if (node->mesh != NULL) {
			meshes.insert(node->mesh);
		}
	}
	return meshes;
}

std::set<GLTF::Primitive*> GLTF::Asset::getAllPrimitives() {
	std::set<GLTF::Primitive*> primitives;
	for (GLTF::Mesh* mesh : getAllMeshes()) {
		for (GLTF::Primitive* primitive : mesh->primitives) {
			primitives.insert(primitive);
		}
	}
	return primitives;
}

std::set<GLTF::Skin*> GLTF::Asset::getAllSkins() {
	std::set<GLTF::Skin*> skins;
	for (GLTF::Node* node : getAllNodes()) {
		GLTF::Skin* skin = node->skin;
		if (skin != NULL) {
			skins.insert(skin);
		}
	}
	return skins;
}

std::set<GLTF::Material*> GLTF::Asset::getAllMaterials() {
	std::set<GLTF::Material*> materials;
	for (GLTF::Primitive* primitive : getAllPrimitives()) {
		GLTF::Material* material = primitive->material;
		if (material != NULL) {
			materials.insert(material);
		}
	}
	return materials;
}

std::set<GLTF::Technique*> GLTF::Asset::getAllTechniques() {
	std::set<GLTF::Technique*> techniques;
	for (GLTF::Material* material : getAllMaterials()) {
		GLTF::Technique* technique = material->technique;
		if (technique != NULL) {
			techniques.insert(technique);
		}
	}
	return techniques;
}

std::set<GLTF::Program*> GLTF::Asset::getAllPrograms() {
	std::set<GLTF::Program*> programs;
	for (GLTF::Technique* technique : getAllTechniques()) {
		GLTF::Program* program = technique->program;
		if (program != NULL) {
			programs.insert(program);
		}
	}
	return programs;
}
std::set<GLTF::Shader*> GLTF::Asset::getAllShaders() {
	std::set<GLTF::Shader*> shaders;
	for (GLTF::Program* program : getAllPrograms()) {
		GLTF::Shader* vertexShader = program->vertexShader;
		if (vertexShader != NULL) {
			shaders.insert(vertexShader);
		}
		GLTF::Shader* fragmentShader = program->fragmentShader;
		if (fragmentShader != NULL) {
			shaders.insert(fragmentShader);
		}
	}
	return shaders;
}

std::set<GLTF::Texture*> GLTF::Asset::getAllTextures() {
	std::set<GLTF::Texture*> textures;
	for (GLTF::Material* material : getAllMaterials()) {
		if (material->type == GLTF::Material::MATERIAL || material->type == GLTF::Material::MATERIAL_COMMON) {
			GLTF::Material::Values* values = material->values;
			if (values->ambientTexture != NULL) {
				textures.insert(values->ambientTexture);
			}
			if (values->diffuseTexture != NULL) {
				textures.insert(values->diffuseTexture);
			}
			if (values->emissionTexture != NULL) {
				textures.insert(values->emissionTexture);
			}
			if (values->specularTexture != NULL) {
				textures.insert(values->specularTexture);
			}
		}
		else if (material->type == GLTF::Material::PBR_METALLIC_ROUGHNESS) {
			GLTF::MaterialPBR* materialPBR = (GLTF::MaterialPBR*)material;
			if (materialPBR->metallicRoughness->baseColorTexture != NULL) {
				textures.insert(materialPBR->metallicRoughness->baseColorTexture->texture);
			}
			if (materialPBR->metallicRoughness->metallicRoughnessTexture != NULL) {
				textures.insert(materialPBR->metallicRoughness->metallicRoughnessTexture->texture);
			}
			if (materialPBR->emissiveTexture != NULL) {
				textures.insert(materialPBR->emissiveTexture->texture);
			}
			if (materialPBR->normalTexture != NULL) {
				textures.insert(materialPBR->normalTexture->texture);
			}
			if (materialPBR->occlusionTexture != NULL) {
				textures.insert(materialPBR->occlusionTexture->texture);
			}
			if (materialPBR->specularGlossiness->diffuseTexture != NULL) {
				textures.insert(materialPBR->specularGlossiness->diffuseTexture->texture);
			}
			if (materialPBR->specularGlossiness->specularGlossinessTexture != NULL) {
				textures.insert(materialPBR->specularGlossiness->specularGlossinessTexture->texture);
			}
		}
	}
	return textures;
}

std::set<GLTF::Image*> GLTF::Asset::getAllImages() {
	std::set<GLTF::Image*> images;
	for (GLTF::Texture* texture : getAllTextures()) {
		GLTF::Image* image = texture->source;
		if (image != NULL) {
			images.insert(image);
		}
	}
	return images;
}

void GLTF::Asset::removeUnusedSemantics() {
	for (GLTF::Primitive* primitive : getAllPrimitives()) {
		GLTF::Material* material = primitive->material;
		if (material != NULL) {
			GLTF::Material::Values* values = material->values;
			std::map<std::string, GLTF::Accessor*> attributes = primitive->attributes;
			for (const auto attribute : attributes) {
				std::string semantic = attribute.first;
				if (semantic.find("TEXCOORD") != std::string::npos) {
					std::map<std::string, GLTF::Accessor*>::iterator removeTexcoord = primitive->attributes.find(semantic);
					if (semantic == "TEXCOORD_0") {
						if (values->ambientTexture == NULL && values->diffuseTexture == NULL && values->emissionTexture == NULL && values->specularTexture == NULL) {
							std::map<std::string, GLTF::Accessor*>::iterator removeTexcoord = primitive->attributes.find(semantic);
							primitive->attributes.erase(removeTexcoord);
						}
					}
					else {
						// Right now we don't support multiple sets of texture coordinates
						primitive->attributes.erase(removeTexcoord);
					}
				}
			}
		}
	}
}

void GLTF::Asset::removeUnusedNodes() {
	std::vector<GLTF::Node*> nodeStack;
	std::set<GLTF::Node*> skinNodes;
	for (GLTF::Skin* skin : getAllSkins()) {
		if (skin->skeleton != NULL) {
			skinNodes.insert(skin->skeleton);
		}
		for (GLTF::Node* jointNode : skin->joints) {
			skinNodes.insert(jointNode);
		}
	}

	for (GLTF::Node* node : getDefaultScene()->nodes) {
		nodeStack.push_back(node);
	}
	while (nodeStack.size() > 0) {
		GLTF::Node* node = nodeStack.back();
		nodeStack.pop_back();
		for (size_t i = 0; i < node->children.size(); i++) {
			GLTF::Node* child = node->children[i];
			if (child->children.size() == 0 && child->mesh == NULL && child->camera == NULL && child->light == NULL && child->skin == NULL) {
				if (std::find(skinNodes.begin(), skinNodes.end(), child) == skinNodes.end()) {
					// this node is extraneous, remove it
					node->children.erase(node->children.begin() + i);
					i--;
					// add the parent back to the node stack for re-evaluation
					nodeStack.push_back(node);
				}
			}
			else {
				nodeStack.push_back(child);
			}
		}
	}
}

GLTF::BufferView* packAccessorsForTargetByteStride(std::vector<GLTF::Accessor*> accessors, GLTF::Constants::WebGL target, size_t byteStride) {
	std::map<GLTF::Accessor*, size_t> byteOffsets;
	size_t byteLength = 0;
	for (GLTF::Accessor* accessor : accessors) {
		int componentByteLength = accessor->getComponentByteLength();
		int padding = byteLength % componentByteLength;
		if (padding != 0) {
			byteLength += (componentByteLength - padding);
		}
		byteOffsets[accessor] = byteLength;
		byteLength += componentByteLength * accessor->getNumberOfComponents() * accessor->count;
	}
	unsigned char* bufferData = new unsigned char[byteLength];
	GLTF::BufferView* bufferView = new GLTF::BufferView(bufferData, byteLength, target);
	for (GLTF::Accessor* accessor : accessors) {
		size_t byteOffset = byteOffsets[accessor];
		GLTF::Accessor* packedAccessor = new GLTF::Accessor(accessor->type, accessor->componentType, byteOffset, accessor->count, bufferView);
		int numberOfComponents = accessor->getNumberOfComponents();
		double* component = new double[numberOfComponents];
		for (int i = 0; i < accessor->count; i++) {
			accessor->getComponentAtIndex(i, component);
			packedAccessor->writeComponentAtIndex(i, component);
		}
		accessor->byteOffset = packedAccessor->byteOffset;
		accessor->bufferView = packedAccessor->bufferView;
	}
	return bufferView;
}

GLTF::Buffer* GLTF::Asset::packAccessors() {
	std::map<GLTF::Constants::WebGL, std::map<int, std::vector<GLTF::Accessor*>>> accessorGroups;
	accessorGroups[GLTF::Constants::WebGL::ARRAY_BUFFER] = std::map<int, std::vector<GLTF::Accessor*>>();
	accessorGroups[GLTF::Constants::WebGL::ELEMENT_ARRAY_BUFFER] = std::map<int, std::vector<GLTF::Accessor*>>();
	accessorGroups[(GLTF::Constants::WebGL)-1] = std::map<int, std::vector<GLTF::Accessor*>>();

	size_t byteLength = 0;
	for (GLTF::Accessor* accessor : getAllAccessors()) {
		GLTF::Constants::WebGL target = accessor->bufferView->target;
		auto targetGroup = accessorGroups[target];
		int byteStride = accessor->getByteStride();
		auto findByteStrideGroup = targetGroup.find(byteStride);
		std::vector<GLTF::Accessor*> byteStrideGroup;
		if (findByteStrideGroup == targetGroup.end()) {
			byteStrideGroup = std::vector<GLTF::Accessor*>();
		}
		else {
			byteStrideGroup = findByteStrideGroup->second;
		}
		byteStrideGroup.push_back(accessor);
		targetGroup[byteStride] = byteStrideGroup;
		accessorGroups[target] = targetGroup;
		byteLength += accessor->bufferView->byteLength;
	}

	std::vector<int> byteStrides;
	std::map<int, std::vector<GLTF::BufferView*>> bufferViews;
	for (auto targetGroup : accessorGroups) {
		for (auto byteStrideGroup : targetGroup.second) {
			GLTF::Constants::WebGL target = targetGroup.first;
			int byteStride = byteStrideGroup.first;
			GLTF::BufferView* bufferView = packAccessorsForTargetByteStride(byteStrideGroup.second, target, byteStride);
			if (target == GLTF::Constants::WebGL::ARRAY_BUFFER) {
				bufferView->byteStride = byteStride;
			}
			auto findBufferViews = bufferViews.find(byteStride);
			std::vector<GLTF::BufferView*> bufferViewGroup;
			if (findBufferViews == bufferViews.end()) {
				byteStrides.push_back(byteStride);
				bufferViewGroup = std::vector<GLTF::BufferView*>();
			}
			else {
				bufferViewGroup = findBufferViews->second;
			}
			bufferViewGroup.push_back(bufferView);
			bufferViews[byteStride] = bufferViewGroup;
		}
	}
	std::sort(byteStrides.begin(), byteStrides.end(), std::greater<int>());

	// Pack these into a buffer sorted from largest byteStride to smallest
	unsigned char* bufferData = new unsigned char[byteLength];
	GLTF::Buffer* buffer = new GLTF::Buffer(bufferData, byteLength);
	size_t byteOffset = 0;
	for (int byteStride : byteStrides) {
		for (GLTF::BufferView* bufferView : bufferViews[byteStride]) {
			std::memcpy(bufferData + byteOffset, bufferView->buffer->data, bufferView->byteLength);
			bufferView->byteOffset = byteOffset;
			bufferView->buffer = buffer;
			byteOffset += bufferView->byteLength;
		}
	}

	return buffer;
}

void GLTF::Asset::writeJSON(void* writer, GLTF::Options* options) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;

	// Write asset metadata
	if (this->metadata) {
		jsonWriter->Key("asset");
		jsonWriter->StartObject();
		this->metadata->writeJSON(writer, options);
		jsonWriter->EndObject();
	}

	// Write scene
	if (this->scene >= 0) {
		jsonWriter->Key("scene");
		jsonWriter->Int(this->scene);
	}

	// Write scenes and build node array
	std::vector<GLTF::Node*> nodes;
	if (this->scenes.size() > 0) {
		jsonWriter->Key("scenes");
		jsonWriter->StartArray();
		for (GLTF::Scene* scene : this->scenes) {
			std::vector<GLTF::Node*> nodeStack;
			for (GLTF::Node* node : scene->nodes) {
				nodeStack.push_back(node);
			}
			while (nodeStack.size() > 0) {
				GLTF::Node* node = nodeStack.back();
				nodeStack.pop_back();
				if (node->id < 0) {
					node->id = nodes.size();
					nodes.push_back(node);
				}
				for (GLTF::Node* child : node->children) {
					nodeStack.push_back(child);
				}
				if (node->skin != NULL) {
					GLTF::Skin* skin = node->skin;
					if (skin->skeleton != NULL) {
						GLTF::Node* skeletonNode = skin->skeleton;
						nodeStack.push_back(skin->skeleton);
					}
				}
			}
			jsonWriter->StartObject();
			scene->writeJSON(writer, options);
			jsonWriter->EndObject();
		}
		jsonWriter->EndArray();
	}

	// Write nodes and build mesh, skin, camera, and light arrays
	std::vector<GLTF::Mesh*> meshes;
	std::vector<GLTF::Skin*> skins;
	std::vector<GLTF::Camera*> cameras;
	std::vector<GLTF::MaterialCommon::Light*> lights;
	if (nodes.size() > 0) {
		jsonWriter->Key("nodes");
		jsonWriter->StartArray();
		for (GLTF::Node* node : nodes) {
			GLTF::Mesh* mesh = node->mesh;
			if (mesh != NULL) {
				if (mesh->id < 0) {
					mesh->id = meshes.size();
					meshes.push_back(mesh);
				}
			}
			if (node->skin != NULL) {
				if (node->skin->id < 0) {
					node->skin->id = skins.size();
					skins.push_back(node->skin);
				}
			}
			if (node->camera != NULL) {
				if (node->camera->id < 0) {
					node->camera->id = cameras.size();
					cameras.push_back(node->camera);
				}
			}
			GLTF::MaterialCommon::Light* light = node->light;
			if (light != NULL && light->id < 0) {
				light->id = lights.size();
				lights.push_back(light);
			}
			jsonWriter->StartObject();
			node->writeJSON(writer, options);
			jsonWriter->EndObject();
		}
		jsonWriter->EndArray();
	}
	nodes.clear();

	// Write cameras
	if (cameras.size() > 0) {
		jsonWriter->Key("cameras");
		jsonWriter->StartArray();
		for (GLTF::Camera* camera : cameras) {
			jsonWriter->StartObject();
			camera->writeJSON(writer, options);
			jsonWriter->EndObject();
		}
		jsonWriter->EndArray();
	}

	// Write meshes and build accessor and material arrays
	std::vector<GLTF::Accessor*> accessors;
	std::vector<GLTF::Material*> materials;
	std::map<std::string, GLTF::Technique*> generatedTechniques;
	if (meshes.size() > 0) {
		jsonWriter->Key("meshes");
		jsonWriter->StartArray();
		for (GLTF::Mesh* mesh : meshes) {
			for (GLTF::Primitive* primitive : mesh->primitives) {
				if (primitive->material && primitive->material->id < 0) {
					GLTF::Material* material = primitive->material;
					if (!options->materialsCommon) {
						if (material->type == GLTF::Material::Type::MATERIAL_COMMON) {
							GLTF::MaterialCommon* materialCommon = (GLTF::MaterialCommon*)material;
							if (options->glsl) {
								std::string techniqueKey = materialCommon->getTechniqueKey();
								std::map<std::string, GLTF::Technique*>::iterator findTechnique = generatedTechniques.find(techniqueKey);
								if (findTechnique != generatedTechniques.end()) {
									material = new GLTF::Material();
									material->name = materialCommon->name;
									material->values = materialCommon->values;
									material->technique = findTechnique->second;
								}
								else {
									bool hasColor = primitive->attributes.find("COLOR_0") != primitive->attributes.end();
									material = materialCommon->getMaterial(lights, hasColor);
									generatedTechniques[techniqueKey] = material->technique;
								}
							}
							else {
								GLTF::MaterialPBR* materialPbr = materialCommon->getMaterialPBR(options->specularGlossiness);
								if (options->lockOcclusionMetallicRoughness && materialPbr->occlusionTexture != NULL) {
									GLTF::MaterialPBR::Texture* metallicRoughnessTexture = new GLTF::MaterialPBR::Texture();
									metallicRoughnessTexture->texture = materialPbr->occlusionTexture->texture;
									materialPbr->metallicRoughness->metallicRoughnessTexture = metallicRoughnessTexture;
								}
								else if (options->metallicRoughnessTexturePaths.size() > 0) {
									std::string metallicRoughnessTexturePath = options->metallicRoughnessTexturePaths[0];
									if (options->metallicRoughnessTexturePaths.size() > 1) {
										size_t index = materials.size();
										if (index < options->metallicRoughnessTexturePaths.size()) {
											metallicRoughnessTexturePath = options->metallicRoughnessTexturePaths[index];
										}
									}
									if (options->metallicRoughnessTexturePaths.size() == 1) {
										metallicRoughnessTexturePath = options->metallicRoughnessTexturePaths[0];
									}
									GLTF::MaterialPBR::Texture* metallicRoughnessTexture = new GLTF::MaterialPBR::Texture();
									GLTF::Image* image = GLTF::Image::load(metallicRoughnessTexturePath);
									std::map<GLTF::Image*, GLTF::Texture*>::iterator textureCacheIt = _pbrTextureCache.find(image);
									GLTF::Texture* texture;
									if (textureCacheIt == _pbrTextureCache.end()) {
										texture = new GLTF::Texture();
										texture->sampler = globalSampler;
										texture->source = image;
										_pbrTextureCache[image] = texture;
									}
									else {
										texture = textureCacheIt->second;
									}
									metallicRoughnessTexture->texture = texture;
									materialPbr->metallicRoughness->metallicRoughnessTexture = metallicRoughnessTexture;
								}
								material = materialPbr;
							}
						}
					}
					primitive->material = material;
					material->id = materials.size();
					materials.push_back(material);
				}
				if (primitive->indices) {
					GLTF::Accessor* indices = primitive->indices;
					if (indices->id < 0) {
						indices->id = accessors.size();
						accessors.push_back(indices);
					}
				}
				for (auto const& primitiveAttribute : primitive->attributes) {
					GLTF::Accessor* attribute = primitiveAttribute.second;
					if (attribute->id < 0) {
						attribute->id = accessors.size();
						accessors.push_back(attribute);
					}
				}
			}
			jsonWriter->StartObject();
			mesh->writeJSON(writer, options);
			jsonWriter->EndObject();
		}
		jsonWriter->EndArray();
	}
	meshes.clear();

	// Write animations and add accessors to the accessor array
	if (animations.size() > 0) {
		jsonWriter->Key("animations");
		jsonWriter->StartArray();
		for (size_t i = 0; i < animations.size(); i++) {
			GLTF::Animation* animation = animations[i];
			int numChannels = 0;
			for (GLTF::Animation::Channel* channel : animation->channels) {
				if (channel->target->node->id >= 0) {
					numChannels++;
					GLTF::Animation::Sampler* sampler = channel->sampler;
					if (sampler->input->id < 0) {
						sampler->input->id = accessors.size();
						accessors.push_back(sampler->input);
					}
					if (sampler->output->id < 0) {
						sampler->output->id = accessors.size();
						accessors.push_back(sampler->output);
					}
				}
			}
			if (numChannels > 0) {
				jsonWriter->StartObject();
				animation->writeJSON(writer, options);
				jsonWriter->EndObject();
			}
		}
		jsonWriter->EndArray();
	}

	// Write skins and add accessors to the accessor array
	if (skins.size() > 0) {
		jsonWriter->Key("skins");
		jsonWriter->StartArray();
		for (GLTF::Skin* skin : skins) {
			if (skin->inverseBindMatrices != NULL && skin->inverseBindMatrices->id < 0) {
				skin->inverseBindMatrices->id = accessors.size();
				accessors.push_back(skin->inverseBindMatrices);
			}
			jsonWriter->StartObject();
			skin->writeJSON(writer, options);
			jsonWriter->EndObject();
		}
		jsonWriter->EndArray();
	}
	skins.clear();

	// Write accessors and add bufferViews to the bufferView array
	std::vector<GLTF::BufferView*> bufferViews;
	if (accessors.size() > 0) {
		jsonWriter->Key("accessors");
		jsonWriter->StartArray();
		for (GLTF::Accessor* accessor : accessors) {
			if (accessor->bufferView) {
				GLTF::BufferView* bufferView = accessor->bufferView;
				if (bufferView->id < 0) {
					bufferView->id = bufferViews.size();
					bufferViews.push_back(bufferView);
				}
			}
			jsonWriter->StartObject();
			accessor->writeJSON(writer, options);
			jsonWriter->EndObject();
		}
		jsonWriter->EndArray();
	}
	accessors.clear();

	// Write materials and build technique and texture arrays
	std::vector<GLTF::Technique*> techniques;
	std::vector<GLTF::Texture*> textures;
	bool usesTechniqueWebGL = false;
	bool usesMaterialsCommon = false;
	bool usesSpecularGlossiness = false;
	if (materials.size() > 0) {
		jsonWriter->Key("materials");
		jsonWriter->StartArray();
		for (GLTF::Material* material : materials) {
			if (material->type == GLTF::Material::Type::MATERIAL || material->type == GLTF::Material::Type::MATERIAL_COMMON) {
				if (material->type == GLTF::Material::Type::MATERIAL) {
					GLTF::Technique* technique = material->technique;
					if (technique && technique->id < 0) {
						technique->id = techniques.size();
						techniques.push_back(technique);
					}
					if (!usesTechniqueWebGL) {
						this->extensions.insert("KHR_technique_webgl");
						usesTechniqueWebGL = true;
					}
				}
				else if (material->type == GLTF::Material::Type::MATERIAL_COMMON && !usesMaterialsCommon) {
					this->extensions.insert("KHR_materials_common");
					usesMaterialsCommon = true;
				}
				GLTF::Texture* ambientTexture = material->values->ambientTexture;
				if (ambientTexture != NULL && ambientTexture->id < 0) {
					ambientTexture->id = textures.size();
					textures.push_back(ambientTexture);
				}
				GLTF::Texture* diffuseTexture = material->values->diffuseTexture;
				if (diffuseTexture != NULL && diffuseTexture->id < 0) {
					diffuseTexture->id = textures.size();
					textures.push_back(diffuseTexture);
				}
				GLTF::Texture* emissionTexture = material->values->emissionTexture;
				if (emissionTexture != NULL && emissionTexture->id < 0) {
					emissionTexture->id = textures.size();
					textures.push_back(emissionTexture);
				}
				GLTF::Texture* specularTexture = material->values->specularTexture;
				if (specularTexture != NULL && specularTexture->id < 0) {
					specularTexture->id = textures.size();
					textures.push_back(specularTexture);
				}
			}
			else if (material->type == GLTF::Material::Type::PBR_METALLIC_ROUGHNESS) {
				GLTF::MaterialPBR* materialPBR = (GLTF::MaterialPBR*)material;
				GLTF::MaterialPBR::Texture* baseColorTexture = materialPBR->metallicRoughness->baseColorTexture;
				if (baseColorTexture != NULL && baseColorTexture->texture->id < 0) {
					baseColorTexture->texture->id = textures.size();
					textures.push_back(baseColorTexture->texture);
				}
				GLTF::MaterialPBR::Texture* metallicRoughnessTexture = materialPBR->metallicRoughness->metallicRoughnessTexture;
				if (metallicRoughnessTexture != NULL && metallicRoughnessTexture->texture->id < 0) {
					metallicRoughnessTexture->texture->id = textures.size();
					textures.push_back(metallicRoughnessTexture->texture);
				}
				GLTF::MaterialPBR::Texture* normalTexture = materialPBR->normalTexture;
				if (normalTexture != NULL && normalTexture->texture->id < 0) {
					normalTexture->texture->id = textures.size();
					textures.push_back(normalTexture->texture);
				}
				GLTF::MaterialPBR::Texture* occlusionTexture = materialPBR->occlusionTexture;
				if (occlusionTexture != NULL && occlusionTexture->texture->id < 0) {
					occlusionTexture->texture->id = textures.size();
					textures.push_back(occlusionTexture->texture);
				}
				if (options->specularGlossiness) {
					if (!usesSpecularGlossiness) {
						this->extensions.insert("KHR_materials_pbrSpecularGlossiness");
						usesSpecularGlossiness = true;
					}
					GLTF::MaterialPBR::Texture* diffuseTexture = materialPBR->specularGlossiness->diffuseTexture;
					if (diffuseTexture != NULL && diffuseTexture->texture->id < 0) {
						diffuseTexture->texture->id = textures.size();
						textures.push_back(diffuseTexture->texture);
					}
					GLTF::MaterialPBR::Texture* specularGlossinessTexture = materialPBR->specularGlossiness->specularGlossinessTexture;
					if (specularGlossinessTexture != NULL && specularGlossinessTexture->texture->id < 0) {
						specularGlossinessTexture->texture->id = textures.size();
						textures.push_back(specularGlossinessTexture->texture);
					}
				}
			}
			
			jsonWriter->StartObject();
			material->writeJSON(writer, options);
			jsonWriter->EndObject();
		}
		jsonWriter->EndArray();
	}
	materials.clear();

	// Write lights
	if (usesMaterialsCommon && lights.size() > 0) {
		jsonWriter->Key("extensions");
		jsonWriter->StartObject();
		jsonWriter->Key("KHR_materials_common");
		jsonWriter->StartArray();
		for (GLTF::MaterialCommon::Light* light : lights) {
			jsonWriter->StartObject();
			light->writeJSON(writer, options);
			jsonWriter->EndObject();
		}
		lights.clear();
		jsonWriter->EndArray();
		jsonWriter->EndObject();
	}

	// Write textures and build sampler and image arrays
	std::vector<GLTF::Sampler*> samplers;
	std::vector<GLTF::Image*> images;
	if (textures.size() > 0) {
		jsonWriter->Key("textures");
		jsonWriter->StartArray();
		for (GLTF::Texture* texture : textures) {
			GLTF::Sampler* sampler = texture->sampler;
			if (sampler && sampler->id < 0) {
				sampler->id = samplers.size();
				samplers.push_back(sampler);
			}
			GLTF::Image* source = texture->source;
			if (source && source->id < 0) {
				source->id = images.size();
				images.push_back(source);
			}
			jsonWriter->StartObject();
			texture->writeJSON(writer, options);
			jsonWriter->EndObject();
		}
		jsonWriter->EndArray();
	}
	textures.clear();

	// Write images and add bufferViews if we have them
	if (images.size() > 0) {
		jsonWriter->Key("images");
		jsonWriter->StartArray();
		for (GLTF::Image* image : images) {
			GLTF::BufferView* bufferView = image->bufferView;
			if (bufferView != NULL && bufferView->id < 0) {
				bufferView->id = bufferViews.size();
				bufferViews.push_back(bufferView);
			}
			jsonWriter->StartObject();
			image->writeJSON(writer, options);
			jsonWriter->EndObject();
		}
		jsonWriter->EndArray();
	}
	images.clear();

	// Write samplers
	if (samplers.size() > 0) {
		jsonWriter->Key("samplers");
		jsonWriter->StartArray();
		for (GLTF::Sampler* sampler : samplers) {
			jsonWriter->StartObject();
			sampler->writeJSON(writer, options);
			jsonWriter->EndObject();
		}
		jsonWriter->EndArray();
	}
	samplers.clear();

	// Write techniques and build program array
	std::vector<GLTF::Program*> programs;
	if (techniques.size() > 0) {
		jsonWriter->Key("techniques");
		jsonWriter->StartArray();
		for (GLTF::Technique* technique : techniques) {
			if (technique->program) {
				GLTF::Program* program = technique->program;
				if (program->id < 0) {
					program->id = programs.size();
					programs.push_back(program);
				}
			}
			jsonWriter->StartObject();
			technique->writeJSON(writer, options);
			jsonWriter->EndObject();
		}
		jsonWriter->EndArray();
	}
	techniques.clear();

	// Write programs and build shader array
	std::vector<GLTF::Shader*> shaders;
	if (programs.size() > 0) {
		jsonWriter->Key("programs");
		jsonWriter->StartArray();
		for (GLTF::Program* program : programs) {
			GLTF::Shader* vertexShader = program->vertexShader;
			if (vertexShader != NULL && vertexShader->id < 0) {
				vertexShader->id = shaders.size();
				shaders.push_back(vertexShader);
			}
			GLTF::Shader* fragmentShader = program->fragmentShader;
			if (fragmentShader != NULL && fragmentShader->id < 0) {
				fragmentShader->id = shaders.size();
				shaders.push_back(fragmentShader);
			}
			jsonWriter->StartObject();
			program->writeJSON(writer, options);
			jsonWriter->EndObject();
		}
		jsonWriter->EndArray();
	}
	programs.clear();

	// Write shaders
	if (shaders.size() > 0) {
		jsonWriter->Key("shaders");
		jsonWriter->StartArray();
		for (GLTF::Shader* shader : shaders) {
			jsonWriter->StartObject();
			shader->writeJSON(writer, options);
			jsonWriter->EndObject();
		}
		jsonWriter->EndArray();
	}
	shaders.clear();

	// Write bufferViews and add buffers to the buffer array
	std::vector<GLTF::Buffer*> buffers;
	if (bufferViews.size() > 0) {
		jsonWriter->Key("bufferViews");
		jsonWriter->StartArray();
		for (GLTF::BufferView* bufferView : bufferViews) {
			if (bufferView->buffer) {
				GLTF::Buffer* buffer = bufferView->buffer;
				if (buffer->id < 0) {
					buffer->id = buffers.size();
					buffers.push_back(buffer);
				}
			}
			jsonWriter->StartObject();
			bufferView->writeJSON(writer, options);
			jsonWriter->EndObject();
		}
		jsonWriter->EndArray();
	}
	bufferViews.clear();

	// Write buffers
	if (buffers.size() > 0) {
		jsonWriter->Key("buffers");
		jsonWriter->StartArray();
		for (GLTF::Buffer* buffer : buffers) {
			jsonWriter->StartObject();
			buffer->writeJSON(writer, options);
			jsonWriter->EndObject();
		}
		jsonWriter->EndArray();
	}
	buffers.clear();

	// Write extensionsUsed and extensionsRequired
	if (this->extensions.size() > 0) {
		jsonWriter->Key("extensionsRequired");
		jsonWriter->StartArray();
		for (const std::string extension : this->extensions) {
			jsonWriter->String(extension.c_str());
		}
		jsonWriter->EndArray();
		jsonWriter->Key("extensionsUsed");
		jsonWriter->StartArray();
		for (const std::string extension : this->extensions) {
			jsonWriter->String(extension.c_str());
		}
		jsonWriter->EndArray();
	}

	GLTF::Object::writeJSON(writer, options);
}
