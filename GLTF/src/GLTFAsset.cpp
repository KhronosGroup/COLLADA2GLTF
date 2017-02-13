#include "GLTFAsset.h"

#include <map>

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

GLTF::Asset::Asset() {
	metadata = new GLTF::Asset::Metadata();
	metadata->profile = new GLTF::Asset::Profile();
}

void GLTF::Asset::Profile::writeJSON(void* writer, GLTF::Options* options) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;
	if (api.length() > 0) {
		jsonWriter->Key("api");
		jsonWriter->String(api.c_str());
	}
	if (version.length() > 0) {
		jsonWriter->Key("version");
		jsonWriter->String(version.c_str());
	}
	GLTF::Object::writeJSON(writer, options);
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
	jsonWriter->Key("premultipliedAlpha");
	jsonWriter->Bool(premultipliedAlpha);
	if (profile) {
		jsonWriter->Key("profile");
		jsonWriter->StartObject();
		profile->writeJSON(writer, options);
		jsonWriter->EndObject();
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
		for (GLTF::Node* skeleton : node->skeletons) {
			nodeStack.push_back(skeleton);
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

void GLTF::Asset::separateSkeletonNodes() {
	std::vector<GLTF::Node*> nodeStack;
	std::vector<GLTF::Node*> skinnedNodes;
	std::map<GLTF::Node*, GLTF::Node*> parents;

	for (GLTF::Node* node : getDefaultScene()->nodes) {
		nodeStack.push_back(node);
	}
	while (nodeStack.size() > 0) {
		GLTF::Node* node = nodeStack.back();
		nodeStack.pop_back();
		for (GLTF::Node* child : node->children) {
			parents[child] = node; 
			nodeStack.push_back(child);
		}
		if (node->skeletons.size() > 0) {
			skinnedNodes.push_back(node);
		}
	}
	for (GLTF::Node* skinnedNode : skinnedNodes) {
		std::vector<GLTF::Node*> skeletons = skinnedNode->skeletons;
		for (size_t i = 0; i < skeletons.size(); i++) {
			GLTF::Node* skeletonNode = skeletons[i];
			GLTF::Node::TransformMatrix* transform = NULL;
			auto it = parents.find(skeletonNode);
			GLTF::Node* skeletonParent = NULL;
			while (it != parents.end()) {
				GLTF::Node* parent = it->second;
				if (transform == NULL) {
					skeletonParent = parent;
					transform = new GLTF::Node::TransformMatrix();
				}
				transform->premultiply((GLTF::Node::TransformMatrix*)parent->transform);
				it = parents.find(parent);
			}
			if (transform != NULL) {
				auto removeChild = std::find(skeletonParent->children.begin(), skeletonParent->children.end(), skeletonNode);
				if (removeChild != skeletonParent->children.end()) {
					skeletonParent->children.erase(removeChild);
				}
				if (!transform->isIdentity()) {
					GLTF::Node* newRoot = new GLTF::Node();
					newRoot->transform = transform;
					newRoot->children.push_back(skeletonNode);
					skinnedNode->skeletons[i] = newRoot;
				}
			}
		}
	}
}

void GLTF::Asset::removeUnusedNodes() {
	std::vector<GLTF::Node*> nodeStack;

	for (GLTF::Node* node : getDefaultScene()->nodes) {
		nodeStack.push_back(node);
	}
	while (nodeStack.size() > 0) {
		GLTF::Node* node = nodeStack.back();
		nodeStack.pop_back();
		for (size_t i = 0; i < node->children.size(); i++) {
			GLTF::Node* child = node->children[i];
			if (child->children.size() == 0 && child->skeletons.size() == 0 && child->mesh == NULL && child->camera == NULL && child->light == NULL && child->skin == NULL && child->jointName == "") {
				// this node is extraneous, remove it
				node->children.erase(node->children.begin() + i);
				i--;
				// add the parent back to the node stack for re-evaluation
				nodeStack.push_back(node);
			}
			else {
				nodeStack.push_back(child);
			}
		}
	}
}

GLTF::BufferView* packAccessorsForTarget(std::vector<GLTF::Accessor*> accessors, GLTF::Constants::WebGL target) {
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
		GLTF::Accessor* packedAccessor = new GLTF::Accessor(accessor->type, accessor->componentType, byteOffset, 0, accessor->count, bufferView);
		int numberOfComponents = accessor->getNumberOfComponents();
		double* component = new double[numberOfComponents];
		for (int i = 0; i < accessor->count; i++) {
			accessor->getComponentAtIndex(i, component);
			packedAccessor->writeComponentAtIndex(i, component);
		}
		accessor->byteOffset = packedAccessor->byteOffset;
		accessor->byteStride = packedAccessor->byteStride;
		accessor->bufferView = packedAccessor->bufferView;
	}
	return bufferView;
}

GLTF::Buffer* GLTF::Asset::packAccessors() {
	std::vector<GLTF::Accessor*> attributeAccessors;
	std::vector<GLTF::Accessor*> indicesAccessors;
	std::vector<GLTF::Accessor*> animationAccessors;

	for (GLTF::Skin* skin : getAllSkins()) {
		GLTF::Accessor* inverseBindMatrices = skin->inverseBindMatrices;
		if (inverseBindMatrices != NULL) {
			std::vector<GLTF::Accessor*>::iterator it = std::find(attributeAccessors.begin(), attributeAccessors.end(), inverseBindMatrices);
			if (it == attributeAccessors.end()) {
				attributeAccessors.push_back(inverseBindMatrices);
			}
		}
	}

	for (GLTF::Primitive* primitive : getAllPrimitives()) {
		for (const auto attribute : primitive->attributes) {
			GLTF::Accessor* attributeAccessor = attribute.second;
			std::vector<GLTF::Accessor*>::iterator it = std::find(attributeAccessors.begin(), attributeAccessors.end(), attributeAccessor);
			if (it == attributeAccessors.end()) {
				attributeAccessors.push_back(attributeAccessor);
			}
		}
		GLTF::Accessor* indicesAccessor = primitive->indices;
		if (indicesAccessor != NULL) {
			std::vector<GLTF::Accessor*>::iterator it = std::find(indicesAccessors.begin(), indicesAccessors.end(), indicesAccessor);
			if (it == indicesAccessors.end()) {
				indicesAccessors.push_back(indicesAccessor);
			}
		}
	}

	for (GLTF::Animation* animation : animations) {
		for (GLTF::Animation::Channel* channel : animation->channels) {
			GLTF::Animation::Sampler* sampler = channel->sampler;
			GLTF::Accessor* input = sampler->input;
			std::vector<GLTF::Accessor*>::iterator it = std::find(animationAccessors.begin(), animationAccessors.end(), input);
			if (it == animationAccessors.end()) {
				animationAccessors.push_back(input);
			}
			GLTF::Accessor* output = sampler->output;
			it = std::find(animationAccessors.begin(), animationAccessors.end(), output);
			if (it == animationAccessors.end()) {
				animationAccessors.push_back(output);
			}
		}
	}

	GLTF::BufferView* attributeBufferView = packAccessorsForTarget(attributeAccessors, GLTF::Constants::WebGL::ARRAY_BUFFER);
	GLTF::BufferView* indicesBufferView = packAccessorsForTarget(indicesAccessors, GLTF::Constants::WebGL::ELEMENT_ARRAY_BUFFER);
	GLTF::BufferView* animationBufferView = packAccessorsForTarget(animationAccessors, (GLTF::Constants::WebGL) - 1);

	size_t byteLength = attributeBufferView->byteLength + indicesBufferView->byteLength;
	int padding = byteLength % 4;
	if (padding != 0) {
		padding = 4 - padding;
	}
	byteLength += padding + animationBufferView->byteLength;

	unsigned char* bufferData = new unsigned char[byteLength];
	std::memcpy(bufferData, attributeBufferView->buffer->data, attributeBufferView->byteLength);
	std::memcpy(bufferData + attributeBufferView->byteLength, indicesBufferView->buffer->data, indicesBufferView->byteLength);
	std::memcpy(bufferData + attributeBufferView->byteLength + indicesBufferView->byteLength + padding, animationBufferView->buffer->data, animationBufferView->byteLength);
	
	GLTF::Buffer* buffer = new GLTF::Buffer(bufferData, byteLength);
	attributeBufferView->buffer = buffer;
	indicesBufferView->buffer = buffer;
	indicesBufferView->byteOffset = attributeBufferView->byteLength;
	animationBufferView->buffer = buffer;
	animationBufferView->byteOffset = attributeBufferView->byteLength + indicesBufferView->byteLength + padding;

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
				for (GLTF::Node* skeleton : node->skeletons) {
					nodeStack.push_back(skeleton);
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
				if (primitive->material) {
					GLTF::Material* material = primitive->material;
					if (!options->materialsCommon) {
						if (material->type == GLTF::Material::Type::MATERIAL_COMMON) {
							GLTF::MaterialCommon* materialCommon = (GLTF::MaterialCommon*)material;
							std::string techniqueKey = materialCommon->getTechniqueKey();
							std::map<std::string, GLTF::Technique*>::iterator findTechnique = generatedTechniques.find(techniqueKey);
							if (findTechnique != generatedTechniques.end()) {
								material = new GLTF::Material();
								material->name = materialCommon->name;
								material->values = materialCommon->values;
								material->technique = findTechnique->second;
							}
							else {
								material = ((GLTF::MaterialCommon*)material)->getMaterial(lights);
								generatedTechniques[techniqueKey] = material->technique;
							}
						}
					}
					primitive->material = material;
					if (material->id < 0) {
						material->id = materials.size();
						materials.push_back(material);
					}
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
	bool usesMaterialsCommon = false;
	if (materials.size() > 0) {
		jsonWriter->Key("materials");
		jsonWriter->StartArray();
		for (GLTF::Material* material : materials) {
			if (material->type == GLTF::Material::Type::MATERIAL) {
				GLTF::Technique* technique = material->technique;
				if (technique && technique->id < 0) {
					technique->id = techniques.size();
					techniques.push_back(technique);
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
		if (options->binary) {
			jsonWriter->StartObject();
			jsonWriter->String("binary_glTF");
		}
		else {
			jsonWriter->StartArray();
		}
		for (GLTF::Buffer* buffer : buffers) {
			jsonWriter->StartObject();
			buffer->writeJSON(writer, options);
			jsonWriter->EndObject();
		}
		if (options->binary) {
			jsonWriter->EndObject();
		}
		else {
			jsonWriter->EndArray();
		}
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
