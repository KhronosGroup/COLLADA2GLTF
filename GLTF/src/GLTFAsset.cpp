#include "GLTFAsset.h"

#ifdef TEST_DRACO
#include "mesh_io.h"
#include "obj_encoder.h"
#include <ctime>
#include <chrono>
#include <sstream>
#include <iostream>
#endif

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

std::vector<GLTF::Accessor*> GLTF::Asset::getAllAccessors() {
	std::set<GLTF::Accessor*> uniqueAccessors;
	std::vector<GLTF::Accessor*> accessors;
	for (GLTF::Skin* skin : getAllSkins()) {
		GLTF::Accessor* inverseBindMatrices = skin->inverseBindMatrices;
		if (inverseBindMatrices != NULL) {
			if (uniqueAccessors.find(inverseBindMatrices) == uniqueAccessors.end()) {
				accessors.push_back(inverseBindMatrices);
				uniqueAccessors.insert(inverseBindMatrices);
			}
		}
	}

	for (GLTF::Primitive* primitive : getAllPrimitives()) {
		for (const auto attribute : primitive->attributes) {
			if (uniqueAccessors.find(attribute.second) == uniqueAccessors.end()) {
				accessors.push_back(attribute.second);
				uniqueAccessors.insert(attribute.second);
			}
		}
		GLTF::Accessor* indicesAccessor = primitive->indices;
		if (indicesAccessor != NULL) {
			if (uniqueAccessors.find(indicesAccessor) == uniqueAccessors.end()) {
				accessors.push_back(indicesAccessor);
				uniqueAccessors.insert(indicesAccessor);
			}
		}
	}

	for (GLTF::Animation* animation : animations) {
		for (GLTF::Animation::Channel* channel : animation->channels) {
			GLTF::Animation::Sampler* sampler = channel->sampler;
			if (uniqueAccessors.find(sampler->input) == uniqueAccessors.end()) {
				accessors.push_back(sampler->input);
				uniqueAccessors.insert(sampler->input);
			}
			if (uniqueAccessors.find(sampler->output) == uniqueAccessors.end()) {
				accessors.push_back(sampler->output);
				uniqueAccessors.insert(sampler->input);
			}
		}
	}
	return accessors;
}

std::vector<GLTF::Node*> GLTF::Asset::getAllNodes() {
	std::vector<GLTF::Node*> nodeStack;
	std::vector<GLTF::Node*> nodes;
	std::set<GLTF::Node*> uniqueNodes;
	for (GLTF::Node* node : getDefaultScene()->nodes) {
		nodeStack.push_back(node);
	}
	while (nodeStack.size() > 0) {
		GLTF::Node* node = nodeStack.back();
		if (uniqueNodes.find(node) == uniqueNodes.end()) {
			nodes.push_back(node);
			uniqueNodes.insert(node);
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

std::vector<GLTF::Mesh*> GLTF::Asset::getAllMeshes() {
	std::vector<GLTF::Mesh*> meshes;
	std::set<GLTF::Mesh*> uniqueMeshes;
	for (GLTF::Node* node : getAllNodes()) {
		if (node->mesh != NULL) {
			if (uniqueMeshes.find(node->mesh) == uniqueMeshes.end()) {
				meshes.push_back(node->mesh);
				uniqueMeshes.insert(node->mesh);
			}
		}
	}
	return meshes;
}

std::vector<GLTF::Primitive*> GLTF::Asset::getAllPrimitives() {
	std::vector<GLTF::Primitive*> primitives;
	std::set<GLTF::Primitive*> uniquePrimitives;
	for (GLTF::Mesh* mesh : getAllMeshes()) {
		for (GLTF::Primitive* primitive : mesh->primitives) {
			if (uniquePrimitives.find(primitive) == uniquePrimitives.end()) {
				primitives.push_back(primitive);
				uniquePrimitives.insert(primitive);
			}
		}
	}
	return primitives;
}

std::vector<GLTF::Skin*> GLTF::Asset::getAllSkins() {
	std::vector<GLTF::Skin*> skins;
	std::set<GLTF::Skin*> uniqueSkins;
	for (GLTF::Node* node : getAllNodes()) {
		GLTF::Skin* skin = node->skin;
		if (skin != NULL) {
			if (uniqueSkins.find(skin) == uniqueSkins.end()) {
				skins.push_back(skin);
				uniqueSkins.insert(skin);
			}
		}
	}
	return skins;
}

std::vector<GLTF::Material*> GLTF::Asset::getAllMaterials() {
	std::vector<GLTF::Material*> materials;
	std::set<GLTF::Material*> uniqueMaterials;
	for (GLTF::Primitive* primitive : getAllPrimitives()) {
		GLTF::Material* material = primitive->material;
		if (material != NULL) {
			if (uniqueMaterials.find(material) == uniqueMaterials.end()) {
				materials.push_back(material);
				uniqueMaterials.insert(material);
			}
		}
	}
	return materials;
}

std::vector<GLTF::Technique*> GLTF::Asset::getAllTechniques() {
	std::vector<GLTF::Technique*> techniques;
	std::set<GLTF::Technique*> uniqueTechniques;
	for (GLTF::Material* material : getAllMaterials()) {
		GLTF::Technique* technique = material->technique;
		if (technique != NULL) {
			if (uniqueTechniques.find(technique) == uniqueTechniques.end()) {
				techniques.push_back(technique);
				uniqueTechniques.insert(technique);
			}
		}
	}
	return techniques;
}

std::vector<GLTF::Program*> GLTF::Asset::getAllPrograms() {
	std::vector<GLTF::Program*> programs;
	std::set<GLTF::Program*> uniquePrograms;
	for (GLTF::Technique* technique : getAllTechniques()) {
		GLTF::Program* program = technique->program;
		if (program != NULL) {
			if (uniquePrograms.find(program) == uniquePrograms.end()) {
				programs.push_back(program);
				uniquePrograms.insert(program);
			}
		}
	}
	return programs;
}
std::vector<GLTF::Shader*> GLTF::Asset::getAllShaders() {
	std::vector<GLTF::Shader*> shaders;
	std::set<GLTF::Shader*> uniqueShaders;
	for (GLTF::Program* program : getAllPrograms()) {
		GLTF::Shader* vertexShader = program->vertexShader;
		if (vertexShader != NULL) {
			if (uniqueShaders.find(vertexShader) == uniqueShaders.end()) {
				shaders.push_back(vertexShader);
				uniqueShaders.insert(vertexShader);
			}
		}
		GLTF::Shader* fragmentShader = program->fragmentShader;
		if (fragmentShader != NULL) {
			if (uniqueShaders.find(vertexShader) == uniqueShaders.end()) {
				shaders.push_back(fragmentShader);
				uniqueShaders.insert(fragmentShader);
			}
		}
	}
	return shaders;
}

std::vector<GLTF::Texture*> GLTF::Asset::getAllTextures() {
	std::vector<GLTF::Texture*> textures;
	std::set<GLTF::Texture*> uniqueTextures;
	for (GLTF::Material* material : getAllMaterials()) {
		if (material->type == GLTF::Material::MATERIAL || material->type == GLTF::Material::MATERIAL_COMMON) {
			GLTF::Material::Values* values = material->values;
			if (values->ambientTexture != NULL) {
				if (uniqueTextures.find(values->ambientTexture) == uniqueTextures.end()) {
					textures.push_back(values->ambientTexture);
					uniqueTextures.insert(values->ambientTexture);
				}
			}
			if (values->diffuseTexture != NULL) {
				if (uniqueTextures.find(values->diffuseTexture) == uniqueTextures.end()) {
					textures.push_back(values->diffuseTexture);
					uniqueTextures.insert(values->diffuseTexture);
				}
			}
			if (values->emissionTexture != NULL) {
				if (uniqueTextures.find(values->emissionTexture) == uniqueTextures.end()) {
					textures.push_back(values->emissionTexture);
					uniqueTextures.insert(values->emissionTexture);
				}
			}
			if (values->specularTexture != NULL) {
				if (uniqueTextures.find(values->specularTexture) == uniqueTextures.end()) {
					textures.push_back(values->specularTexture);
					uniqueTextures.insert(values->specularTexture);
				}
			}
		}
		else if (material->type == GLTF::Material::PBR_METALLIC_ROUGHNESS) {
			GLTF::MaterialPBR* materialPBR = (GLTF::MaterialPBR*)material;
			if (materialPBR->metallicRoughness->baseColorTexture != NULL) {
				if (uniqueTextures.find(materialPBR->metallicRoughness->baseColorTexture->texture) == uniqueTextures.end()) {
					textures.push_back(materialPBR->metallicRoughness->baseColorTexture->texture);
					uniqueTextures.insert(materialPBR->metallicRoughness->baseColorTexture->texture);
				}
			}
			if (materialPBR->metallicRoughness->metallicRoughnessTexture != NULL) {
				if (uniqueTextures.find(materialPBR->metallicRoughness->metallicRoughnessTexture->texture) == uniqueTextures.end()) {
					textures.push_back(materialPBR->metallicRoughness->metallicRoughnessTexture->texture);
					uniqueTextures.insert(materialPBR->metallicRoughness->metallicRoughnessTexture->texture);
				}
			}
			if (materialPBR->emissiveTexture != NULL) {
				if (uniqueTextures.find(materialPBR->emissiveTexture->texture) == uniqueTextures.end()) {
					textures.push_back(materialPBR->emissiveTexture->texture);
					uniqueTextures.insert(materialPBR->emissiveTexture->texture);
				}
			}
			if (materialPBR->normalTexture != NULL) {
				if (uniqueTextures.find(materialPBR->normalTexture->texture) == uniqueTextures.end()) {
					textures.push_back(materialPBR->normalTexture->texture);
					uniqueTextures.insert(materialPBR->normalTexture->texture);
				}
			}
			if (materialPBR->occlusionTexture != NULL) {
				if (uniqueTextures.find(materialPBR->occlusionTexture->texture) == uniqueTextures.end()) {
					textures.push_back(materialPBR->occlusionTexture->texture);
					uniqueTextures.insert(materialPBR->occlusionTexture->texture);
				}
			}
			if (materialPBR->specularGlossiness->diffuseTexture != NULL) {
				if (uniqueTextures.find(materialPBR->specularGlossiness->diffuseTexture->texture) == uniqueTextures.end()) {
					textures.push_back(materialPBR->specularGlossiness->diffuseTexture->texture);
					uniqueTextures.insert(materialPBR->specularGlossiness->diffuseTexture->texture);
				}
			}
			if (materialPBR->specularGlossiness->specularGlossinessTexture != NULL) {
				if (uniqueTextures.find(materialPBR->specularGlossiness->specularGlossinessTexture->texture) == uniqueTextures.end()) {
					textures.push_back(materialPBR->specularGlossiness->specularGlossinessTexture->texture);
					uniqueTextures.insert(materialPBR->specularGlossiness->specularGlossinessTexture->texture);
				}
			}
		}
	}
	return textures;
}

std::vector<GLTF::Image*> GLTF::Asset::getAllImages() {
	std::vector<GLTF::Image*> images;
	std::set<GLTF::Image*> uniqueImages;
	for (GLTF::Texture* texture : getAllTextures()) {
		GLTF::Image* image = texture->source;
		if (image != NULL) {
			if (uniqueImages.find(image) == uniqueImages.end()) {
				images.push_back(image);
				uniqueImages.insert(image);
			}
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

bool isUnusedNode(GLTF::Node* node, std::set<GLTF::Node*> skinNodes, bool isPbr) {
	if (node->children.size() == 0 && node->mesh == NULL && node->camera == NULL && node->skin == NULL) {
		if (isPbr || node->light == NULL || node->light->type == GLTF::MaterialCommon::Light::AMBIENT) {
			if (std::find(skinNodes.begin(), skinNodes.end(), node) == skinNodes.end()) {
				return true;
			}
		}
	}
	return false;
}

void GLTF::Asset::removeUnusedNodes(GLTF::Options* options) {
	std::vector<GLTF::Node*> nodeStack;
	std::set<GLTF::Node*> skinNodes;
	bool isPbr = !options->glsl && !options->materialsCommon;
	for (GLTF::Skin* skin : getAllSkins()) {
		if (skin->skeleton != NULL) {
			skinNodes.insert(skin->skeleton);
		}
		for (GLTF::Node* jointNode : skin->joints) {
			skinNodes.insert(jointNode);
		}
	}

	GLTF::Scene* defaultScene = getDefaultScene();
	bool needsPass = true;
	while (needsPass) {
		needsPass = false;
		for (size_t i = 0; i < defaultScene->nodes.size(); i++) {
			GLTF::Node* node = defaultScene->nodes[i];
			if (isUnusedNode(node, skinNodes, isPbr)) {
				defaultScene->nodes.erase(defaultScene->nodes.begin() + i);
				i--;
			}
			else {
				nodeStack.push_back(node);
			}
		}
		while (nodeStack.size() > 0) {
			GLTF::Node* node = nodeStack.back();
			nodeStack.pop_back();
			for (size_t i = 0; i < node->children.size(); i++) {
				GLTF::Node* child = node->children[i];
				if (isUnusedNode(child, skinNodes, isPbr)) {
					// this node is extraneous, remove it
					node->children.erase(node->children.begin() + i);
					i--;
					if (node->children.size() == 0) {
						// another pass may be required to clean up the parent
						needsPass = true;
					}
				}
				else {
					nodeStack.push_back(child);
				}
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
		float* component = new float[numberOfComponents];
		for (int i = 0; i < accessor->count; i++) {
			accessor->getComponentAtIndex(i, component);
			packedAccessor->writeComponentAtIndex(i, component);
		}
		accessor->byteOffset = packedAccessor->byteOffset;
		accessor->bufferView = packedAccessor->bufferView;
	}
	return bufferView;
}

#ifdef USE_DRACO
bool GLTF::Asset::compressPrimitives() {
	for (GLTF::Primitive* primitive : getAllPrimitives()) {
    auto draco_ext_itr = primitive->extensions.find("KHR_draco_mesh_compression");
    if (draco_ext_itr == primitive->extensions.end()) {
      // No extension exists.
      continue;
    }
    GLTF::DracoExtension* draco_extension = (GLTF::DracoExtension*)draco_ext_itr->second;
    draco::Mesh *draco_mesh = draco_extension->draco_mesh.get();

#ifdef TEST_DRACO
 
    draco::ObjEncoder obj_encoder;
    std::time_t result = std::time(nullptr);
    std::ostringstream oss;
    oss << "draco_" << result << ".obj";
    if (!obj_encoder.EncodeToFile(*draco_mesh, oss.str())) {
      std::cerr << "Error: write to obj file.\n";
    }
#endif
    // Compress the mesh
    // Setup encoder options.
    draco::Encoder encoder;
    int pos_quantization_bits= 14;
    int tex_coords_quantization_bits = 10;
    int normals_quantization_bits = 10;
    int generic_quantization_bits = 8;
    encoder.SetAttributeQuantization(draco::GeometryAttribute::POSITION,
                                     pos_quantization_bits);
    encoder.SetAttributeQuantization(draco::GeometryAttribute::TEX_COORD,
                                     tex_coords_quantization_bits);
    encoder.SetAttributeQuantization(draco::GeometryAttribute::NORMAL,
                                     normals_quantization_bits);
    //encoder.SetAttributeQuantization(draco::GeometryAttribute::GENERIC,
    //                                 normals_quantization_bits);
    const int speed = 5;
    encoder.SetSpeedOptions(speed, speed);

    // std::cout << "Mesh now has " << draco_mesh->num_attributes() << " attributes.\n";
    draco::EncoderBuffer buffer;
    const draco::Status status = encoder.EncodeMeshToBuffer(*draco_mesh, &buffer);
    if (!status.ok()) {
      std::cerr << "Error: Encode mesh.\n";
      return false;
    }
     
/*
    // Test Decoding
    draco::DecoderBuffer decoder_buffer;
    decoder_buffer.Init(buffer.data(), buffer.size());
    const draco::EncodedGeometryType geom_type = draco::GetEncodedGeometryType(&decoder_buffer);
    if (geom_type == draco::TRIANGULAR_MESH) {
      std::unique_ptr<draco::Mesh> in_mesh = draco::DecodeMeshFromBuffer(&decoder_buffer);
      if (in_mesh) {
        std::cout << "Decoded Mesh has " << in_mesh->num_attributes() << " attributes.\n";
      }
    }
*/

    // Add data to bufferview
    unsigned char* allocatedData = (unsigned char*)malloc(buffer.size());
    std::memcpy(allocatedData, buffer.data(), buffer.size());
    GLTF::BufferView* bufferView = new GLTF::BufferView(allocatedData, buffer.size());
    draco_extension->bufferView = bufferView;
    // std::cout << "Done! Encoded mesh of size " << buffer.size() << ".\n";
  }
  return true;
}


GLTF::Buffer* GLTF::Asset::packAccessorsWithCompressedAssets() {
	std::set<GLTF::Accessor*> uniqueAccessors;
	std::vector<GLTF::Accessor*> accessors;
	for (GLTF::Skin* skin : getAllSkins()) {
		GLTF::Accessor* inverseBindMatrices = skin->inverseBindMatrices;
		if (inverseBindMatrices != NULL) {
			if (uniqueAccessors.find(inverseBindMatrices) == uniqueAccessors.end()) {
				accessors.push_back(inverseBindMatrices);
				uniqueAccessors.insert(inverseBindMatrices);
			}
		}
	}

	std::vector<GLTF::BufferView*> compressedBufferViews;
	for (GLTF::Primitive* primitive : getAllPrimitives()) {
    // TODO: If some attribute is left not compressed.
    auto draco_ext_itr = primitive->extensions.find("KHR_draco_mesh_compression");
    if (draco_ext_itr != primitive->extensions.end()) {
      compressedBufferViews.push_back(((GLTF::DracoExtension*)draco_ext_itr->second)->bufferView);
      continue;
    }
		for (const auto attribute : primitive->attributes) {
			if (uniqueAccessors.find(attribute.second) == uniqueAccessors.end()) {
				accessors.push_back(attribute.second);
				uniqueAccessors.insert(attribute.second);
			}
		}
		GLTF::Accessor* indicesAccessor = primitive->indices;
		if (indicesAccessor != NULL) {
			if (uniqueAccessors.find(indicesAccessor) == uniqueAccessors.end()) {
				accessors.push_back(indicesAccessor);
				uniqueAccessors.insert(indicesAccessor);
			}
		}
	}

	for (GLTF::Animation* animation : animations) {
		for (GLTF::Animation::Channel* channel : animation->channels) {
			GLTF::Animation::Sampler* sampler = channel->sampler;
			if (uniqueAccessors.find(sampler->input) == uniqueAccessors.end()) {
				accessors.push_back(sampler->input);
				uniqueAccessors.insert(sampler->input);
			}
			if (uniqueAccessors.find(sampler->output) == uniqueAccessors.end()) {
				accessors.push_back(sampler->output);
				uniqueAccessors.insert(sampler->input);
			}
		}
	}

	std::map<GLTF::Constants::WebGL, std::map<int, std::vector<GLTF::Accessor*>>> accessorGroups;
	accessorGroups[GLTF::Constants::WebGL::ARRAY_BUFFER] = std::map<int, std::vector<GLTF::Accessor*>>();
	accessorGroups[GLTF::Constants::WebGL::ELEMENT_ARRAY_BUFFER] = std::map<int, std::vector<GLTF::Accessor*>>();
	accessorGroups[(GLTF::Constants::WebGL)-1] = std::map<int, std::vector<GLTF::Accessor*>>();

	size_t byteLength = 0;
	for (GLTF::Accessor* accessor : accessors) {
    if (!accessor->bufferView) { continue;
    }
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
  // Add length of compressed data.
  for (GLTF::BufferView* compressedBufferView : compressedBufferViews) {
    byteLength += compressedBufferView->byteLength;
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

  for (GLTF::BufferView* compressedBufferView : compressedBufferViews) {
    std::memcpy(bufferData + byteOffset,
        compressedBufferView->buffer->data, compressedBufferView->byteLength);
    compressedBufferView->byteOffset = byteOffset;
    compressedBufferView->buffer = buffer;
    byteOffset += compressedBufferView->byteLength;
  }

	return buffer;
}
#endif

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

void GLTF::Asset::requireExtension(std::string extension) {
	useExtension(extension);
	extensionsRequired.insert(extension);
}

void GLTF::Asset::useExtension(std::string extension) {
	extensionsUsed.insert(extension);
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
	std::vector<GLTF::BufferView*> bufferViews;
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
#ifdef USE_DRACO
        // BufferView of compressed data does not belong to Accessors.
        auto draco_ext_itr = primitive->extensions.find("KHR_draco_mesh_compression");
        if (draco_ext_itr != primitive->extensions.end()) {
          GLTF::BufferView* bufferView = ((GLTF::DracoExtension*)draco_ext_itr->second)->bufferView;
          if (bufferView->id < 0) {
            bufferView->id = bufferViews.size();
            bufferViews.push_back(bufferView);
          }
          continue;
        }
#endif
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
//#ifdef USE_DRACO
  if (options->dracoCompression) {
    this->requireExtension("KHR_draco_mesh_compression");
  }
//#endif
	meshes.clear();
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
						this->requireExtension("KHR_technique_webgl");
						usesTechniqueWebGL = true;
					}
				}
				else if (material->type == GLTF::Material::Type::MATERIAL_COMMON && !usesMaterialsCommon) {
					this->requireExtension("KHR_materials_common");
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
						this->useExtension("KHR_materials_pbrSpecularGlossiness");
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
	if (this->extensionsRequired.size() > 0) {
		jsonWriter->Key("extensionsRequired");
		jsonWriter->StartArray();
		for (const std::string extension : this->extensionsRequired) {
			jsonWriter->String(extension.c_str());
		}
		jsonWriter->EndArray();
	}
	if (this->extensionsUsed.size() > 0) {
		jsonWriter->Key("extensionsUsed");
		jsonWriter->StartArray();
		for (const std::string extension : this->extensionsUsed) {
			jsonWriter->String(extension.c_str());
		}
		jsonWriter->EndArray();
	}

	GLTF::Object::writeJSON(writer, options);
}
