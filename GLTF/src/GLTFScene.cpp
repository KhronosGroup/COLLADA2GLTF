#include "GLTFScene.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

std::string GLTF::Scene::typeName() {
	return "scene";
}

void GLTF::Scene::writeJSON(void* writer, GLTF::Options* options) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;
	jsonWriter->Key("nodes");
	jsonWriter->StartArray();
	for (GLTF::Node* node : this->nodes) {
		if (options->version == "1.0") {
			jsonWriter->String(node->getStringId().c_str());
		}
		else {
			jsonWriter->Int(node->id);
		}
	}
	jsonWriter->EndArray();
	GLTF::Object::writeJSON(writer, options);
}
