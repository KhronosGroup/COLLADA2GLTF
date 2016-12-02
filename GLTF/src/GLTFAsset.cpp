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
	if (this->scene > 0) {
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
			for (GLTF::Node* node : scene->nodes) {
				nodes[node->id] = node;
			}
			jsonWriter->Key(scene->id.c_str());
			jsonWriter->StartObject();
			scene->writeJSON(writer);
			jsonWriter->EndObject();
		}
		jsonWriter->EndObject();
	}

	// Write nodes and build mesh hash
	std::map<std::string, GLTF::Mesh*> meshes;
	if (nodes.size() > 0) {
		jsonWriter->Key("nodes");
		jsonWriter->StartObject();
		for (auto const& nodeMapEntry : nodes) {
			std::string nodeId = nodeMapEntry.first;
			GLTF::Node* node = nodeMapEntry.second;
			for (GLTF::Mesh* mesh : node->meshes) {
				meshes[mesh->id] = mesh;
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

	// Write materials and build technique hash
	/*std::map<std::string, GLTF::Technique*> techniques;
	if (materials.size() > 0) {
		jsonWriter->Key("materials");
		jsonWriter->StartObject();
		for (auto const& materialMapEntry : materials) {
			std::string materialId = materialMapEntry.first;
			GLTF::Material* material = materialMapEntry.second;
			if (material->technique) {
				GLTF::Technique* technique = material->technique;
				techniques[technique->id] = technique;
			}
			jsonWriter->Key(materialId.c_str());
			jsonWriter->StartObject();
			material->writeJSON(writer);
			jsonWriter->EndObject();
		}
		jsonWriter->EndObject();
	}
	materials.clear();

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
	techniques.clear();*/

	GLTF::Object::writeJSON(writer);
}
