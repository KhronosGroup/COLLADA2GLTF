#include "GLTFMesh.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

int GLTF::Mesh::INSTANCE_COUNT = 0;

GLTF::Mesh::Mesh() {
	this->id = "mesh_" + std::to_string(GLTF::Mesh::INSTANCE_COUNT);
	GLTF::Mesh::INSTANCE_COUNT++;
}

GLTF::Object* GLTF::Mesh::clone() {
	GLTF::Mesh* clone = (GLTF::Mesh*)GLTF::Object::clone();
	for (GLTF::Primitive* primitive : this->primitives) {
		clone->primitives.push_back((GLTF::Primitive*)primitive->clone());
	}
	return clone;
}

void GLTF::Mesh::writeJSON(void* writer) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;
	jsonWriter->Key("primitives");
	jsonWriter->StartArray();
	for (GLTF::Primitive* primitive : this->primitives) {
		jsonWriter->StartObject();
		primitive->writeJSON(jsonWriter);
		jsonWriter->EndObject();
	}
	jsonWriter->EndArray();
	GLTF::Object::writeJSON(writer);
}