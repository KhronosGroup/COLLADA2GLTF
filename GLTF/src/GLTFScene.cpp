#include "GLTFScene.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

void GLTF::Scene::writeJSON(void* writer, GLTF::Options* options) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;
	jsonWriter->Key("nodes");
	jsonWriter->StartArray();
	for (GLTF::Node* node : this->nodes) {
		jsonWriter->Int(node->id);
	}
	jsonWriter->EndArray();
	GLTF::Object::writeJSON(writer, options);
}
