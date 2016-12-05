#include "GLTFNode.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

int GLTF::Node::INSTANCE_COUNT = 0;

GLTF::Node::Node() {
	this->id = "node_" + std::to_string(GLTF::Node::INSTANCE_COUNT);
	GLTF::Accessor::INSTANCE_COUNT++;
}

GLTF::Node::Node(std::string id) {
	this->id = id;
}

void GLTF::Node::writeJSON(void* writer) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;
	jsonWriter->Key("meshes");
	jsonWriter->StartArray();
	for (GLTF::Mesh* mesh : meshes) {
		jsonWriter->String(mesh->id.c_str());
	}
	jsonWriter->EndArray();
}