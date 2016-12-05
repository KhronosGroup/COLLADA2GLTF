#include "GLTFScene.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

int GLTF::Scene::INSTANCE_COUNT = 0;

GLTF::Scene::Scene() {
	this->id = "scene_" + std::to_string(GLTF::Scene::INSTANCE_COUNT);
	GLTF::Scene::INSTANCE_COUNT++;
}

GLTF::Scene::Scene(std::string id) {
	this->id = id;
}

void GLTF::Scene::writeJSON(void* writer) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;
	jsonWriter->Key("nodes");
	jsonWriter->StartArray();
	for (GLTF::Node* node : this->nodes) {
		jsonWriter->String(node->id.c_str());
	}
	jsonWriter->EndArray();
	GLTF::Object::writeJSON(writer);
}
