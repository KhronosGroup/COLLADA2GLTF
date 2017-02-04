#include "GLTFAsset.h"

#include <map>

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

void GLTF::Asset::Profile::writeJSON(void* writer) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;
	if (api.length() > 0) {
		jsonWriter->Key("api");
		jsonWriter->String(api.c_str());
	}
	if (version.length() > 0) {
		jsonWriter->Key("version");
		jsonWriter->String(version.c_str());
	}
	GLTF::Object::writeJSON(writer);
}

void GLTF::Asset::Metadata::writeJSON(void* writer) {
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
		profile->writeJSON(writer);
		jsonWriter->EndObject();
	}
	if (version.length() > 0) {
		jsonWriter->Key("version");
		jsonWriter->String(version.c_str());
	}
	GLTF::Object::writeJSON(writer);
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

void GLTF::Asset::writeJSON(void* writer) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;

	// Write asset metadata
	if (this->metadata) {
		jsonWriter->Key("asset");
		jsonWriter->StartObject();
		this->metadata->writeJSON(writer);
		jsonWriter->EndObject();
	}

	// Write scene
	if (this->scene >= 0) {
		GLTF::Scene* scene = this->scenes[this->scene];
		jsonWriter->Key("scene");
		jsonWriter->String(scene->id.c_str());
	}

	// Write scenes and build node hash
	std::map<std::string, GLTF::Node*> nodes;
	if (this->scenes.size() > 0) {
		jsonWriter->Key("scenes");
		jsonWriter->StartObject();
		for (GLTF::Scene* scene : this->scenes) {
			std::vector<GLTF::Node*> nodeStack;
			for (GLTF::Node* node : scene->nodes) {
				nodeStack.push_back(node);
			}
			while (nodeStack.size() > 0) {
				GLTF::Node* node = nodeStack.back();
				nodeStack.pop_back();
				nodes[node->id] = node;
				for (GLTF::Node* child : node->children) {
					nodeStack.push_back(child);
				}
			}
			jsonWriter->Key(scene->id.c_str());
			jsonWriter->StartObject();
			scene->writeJSON(writer);
			jsonWriter->EndObject();
		}
		jsonWriter->EndObject();
	}

	// Write nodes and build mesh and skin hash
	std::map<std::string, GLTF::Mesh*> meshes;
	std::map<std::string, GLTF::Skin*> skins;
	if (nodes.size() > 0) {
		jsonWriter->Key("nodes");
		jsonWriter->StartObject();
		for (auto const& nodeMapEntry : nodes) {
			std::string nodeId = nodeMapEntry.first;
			GLTF::Node* node = nodeMapEntry.second;
			for (GLTF::Mesh* mesh : node->meshes) {
				meshes[mesh->id] = mesh;
			}
			if (node->skin != NULL) {
				skins[node->skin->id] = node->skin;
			}
			jsonWriter->Key(nodeId.c_str());
			jsonWriter->StartObject();
			node->writeJSON(writer);
			jsonWriter->EndObject();
		}
		jsonWriter->EndObject();
	}
	nodes.clear();

	// Write meshes and build accessor and material hash
	std::map<std::string, GLTF::Accessor*> accessors;
	std::map<std::string, GLTF::Material*> materials;
	if (meshes.size() > 0) {
		jsonWriter->Key("meshes");
		jsonWriter->StartObject();
		for (auto const& meshMapEntry : meshes) {
			std::string meshId = meshMapEntry.first;
			GLTF::Mesh* mesh = meshMapEntry.second;
			for (GLTF::Primitive* primitive : mesh->primitives) {
				if (primitive->material) {
					GLTF::Material* material = primitive->material;
					materials[material->id] = material;
				}
				if (primitive->indices) {
					GLTF::Accessor* indices = primitive->indices;
					accessors[indices->id] = indices;
				}
				for (auto const& primitiveAttribute : primitive->attributes) {
					GLTF::Accessor* attribute = primitiveAttribute.second;
					accessors[attribute->id] = attribute;
				}
			}
			jsonWriter->Key(meshId.c_str());
			jsonWriter->StartObject();
			mesh->writeJSON(writer);
			jsonWriter->EndObject();
		}
		jsonWriter->EndObject();
	}
	meshes.clear();

	// Write animations and add accessors to the accessor hash
	if (animations.size() > 0) {
		jsonWriter->Key("animations");
		jsonWriter->StartObject();
		for (GLTF::Animation* animation : animations) {
			for (GLTF::Animation::Channel* channel : animation->channels) {
				GLTF::Animation::Sampler* sampler = channel->sampler;
				accessors[sampler->input->id] = sampler->input;
				accessors[sampler->output->id] = sampler->output;
			}
			jsonWriter->Key(animation->id.c_str());
			jsonWriter->StartObject();
			animation->writeJSON(writer);
			jsonWriter->EndObject();
		}
		jsonWriter->EndObject();
	}

	// Write skins and add accessors to the accessor hash
	if (skins.size() > 0) {
		jsonWriter->Key("skins");
		jsonWriter->StartObject();
		for (auto const& skinMapEntry : skins) {
			std::string skinId = skinMapEntry.first;
			GLTF::Skin* skin = skinMapEntry.second;
			if (skin->inverseBindMatrices != NULL) {
				accessors[skin->inverseBindMatrices->id] = skin->inverseBindMatrices;
			}
			jsonWriter->Key(skinId.c_str());
			jsonWriter->StartObject();
			skin->writeJSON(writer);
			jsonWriter->EndObject();
		}
		jsonWriter->EndObject();
	}
	skins.clear();

	// Write accessors and build bufferView hash
	std::map<std::string, GLTF::BufferView*> bufferViews;
	if (accessors.size() > 0) {
		jsonWriter->Key("accessors");
		jsonWriter->StartObject();
		for (auto const& accessorMapEntry : accessors) {
			std::string accessorId = accessorMapEntry.first;
			GLTF::Accessor* accessor = accessorMapEntry.second;
			if (accessor->bufferView) {
				GLTF::BufferView* bufferView = accessor->bufferView;
				bufferViews[bufferView->id] = bufferView;
			}
			jsonWriter->Key(accessorId.c_str());
			jsonWriter->StartObject();
			accessor->writeJSON(writer);
			jsonWriter->EndObject();
		}
		jsonWriter->EndObject();
	}
	accessors.clear();

	// Write bufferViews and build buffer hash
	std::map<std::string, GLTF::Buffer*> buffers;
	if (bufferViews.size() > 0) {
		jsonWriter->Key("bufferViews");
		jsonWriter->StartObject();
		for (auto const& bufferViewMapEntry : bufferViews) {
			std::string bufferViewId = bufferViewMapEntry.first;
			GLTF::BufferView* bufferView = bufferViewMapEntry.second;
			if (bufferView->buffer) {
				GLTF::Buffer* buffer = bufferView->buffer;
				buffers[buffer->id] = buffer;
			}
			jsonWriter->Key(bufferViewId.c_str());
			jsonWriter->StartObject();
			bufferView->writeJSON(writer);
			jsonWriter->EndObject();
		}
		jsonWriter->EndObject();
	}
	bufferViews.clear();

	// Write buffers
	if (buffers.size() > 0) {
		jsonWriter->Key("buffers");
		jsonWriter->StartObject();
		for (auto const& bufferMapEntry : buffers) {
			std::string bufferId = bufferMapEntry.first;
			GLTF::Buffer* buffer = bufferMapEntry.second;
			jsonWriter->Key(bufferId.c_str());
			jsonWriter->StartObject();
			buffer->writeJSON(writer);
			jsonWriter->EndObject();
		}
		jsonWriter->EndObject();
	}
	buffers.clear();

	// Write materials and build technique and texture hash
	std::map<std::string, GLTF::Technique*> techniques;
	std::map<std::string, GLTF::Texture*> textures;
	if (materials.size() > 0) {
		jsonWriter->Key("materials");
		jsonWriter->StartObject();
		bool usesMaterialsCommon = false;
		for (auto const& materialMapEntry : materials) {
			std::string materialId = materialMapEntry.first;
			GLTF::Material* material = materialMapEntry.second;
			if (material->type == GLTF::Material::Type::MATERIAL) {
				GLTF::Technique* technique = material->technique;
				if (technique) {
					techniques[technique->id] = technique;
				}
			}
			else if (material->type == GLTF::Material::Type::MATERIAL_COMMON && !usesMaterialsCommon) {
				this->extensionsUsed.insert("KHR_materials_common");
				usesMaterialsCommon = true;
			}
			GLTF::Texture* diffuseTexture = material->values->diffuseTexture;
			if (diffuseTexture) {
				textures[diffuseTexture->id] = diffuseTexture;
			}
			jsonWriter->Key(materialId.c_str());
			jsonWriter->StartObject();
			material->writeJSON(writer);
			jsonWriter->EndObject();
		}
		jsonWriter->EndObject();
	}
	materials.clear();

	// Write textures and build sampler and image hash
	std::map<std::string, GLTF::Sampler*> samplers;
	std::map<std::string, GLTF::Image*> images;
	if (textures.size() > 0) {
		jsonWriter->Key("textures");
		jsonWriter->StartObject();
		for (auto const& textureMapEntry : textures) {
			std::string textureId = textureMapEntry.first;
			GLTF::Texture* texture = textureMapEntry.second;
			GLTF::Sampler* sampler = texture->sampler;
			if (sampler) {
				samplers[sampler->id] = sampler;
			}
			GLTF::Image* source = texture->source;
			if (source) {
				images[source->id] = source;
			}
			jsonWriter->Key(textureId.c_str());
			jsonWriter->StartObject();
			texture->writeJSON(writer);
			jsonWriter->EndObject();
		}
		jsonWriter->EndObject();
	}
	textures.clear();

	// Write images
	if (images.size() > 0) {
		jsonWriter->Key("images");
		jsonWriter->StartObject();
		for (auto const& imageMapEntry : images) {
			std::string imageId = imageMapEntry.first;
			GLTF::Image* image = imageMapEntry.second;
			jsonWriter->Key(imageId.c_str());
			jsonWriter->StartObject();
			image->writeJSON(writer);
			jsonWriter->EndObject();
		}
		jsonWriter->EndObject();
	}
	images.clear();

	// Write samplers
	if (samplers.size() > 0) {
		jsonWriter->Key("samplers");
		jsonWriter->StartObject();
		for (auto const& samplerMapEntry : samplers) {
			std::string samplerId = samplerMapEntry.first;
			GLTF::Sampler* sampler = samplerMapEntry.second;
			jsonWriter->Key(samplerId.c_str());
			jsonWriter->StartObject();
			sampler->writeJSON(writer);
			jsonWriter->EndObject();
		}
		jsonWriter->EndObject();
	}
	samplers.clear();

	// Write techniques and build program hash
	std::map<std::string, GLTF::Program*> programs;
	if (techniques.size() > 0) {
		jsonWriter->Key("techniques");
		jsonWriter->StartObject();
		for (auto const& techniqueMapEntry : techniques) {
			std::string techniqueId = techniqueMapEntry.first;
			GLTF::Technique* technique = techniqueMapEntry.second;
			if (technique->program) {
				GLTF::Program* program = technique->program;
				programs[program->id] = program;
			}
			jsonWriter->Key(techniqueId.c_str());
			jsonWriter->StartObject();
			technique->writeJSON(writer);
			jsonWriter->EndObject();
		}
		jsonWriter->EndObject();
	}
	techniques.clear();

	// Write extensionsUsed
	if (this->extensionsUsed.size() > 0) {
		jsonWriter->Key("extensionsUsed");
		jsonWriter->StartArray();
		for (const std::string extensionUsed : this->extensionsUsed) {
			jsonWriter->String(extensionUsed.c_str());
		}
		jsonWriter->EndArray();
	}

	GLTF::Object::writeJSON(writer);
}
