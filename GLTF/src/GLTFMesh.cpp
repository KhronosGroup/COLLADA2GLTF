#include "GLTFMesh.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

GLTF::Object* GLTF::Mesh::clone() {
	GLTF::Mesh* clone = (GLTF::Mesh*)GLTF::Object::clone();
	for (GLTF::Primitive* primitive : this->primitives) {
		clone->primitives.push_back((GLTF::Primitive*)primitive->clone());
	}
	return clone;
}

void GLTF::Mesh::writeJSON(void* writer, GLTF::Options* options) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;
	jsonWriter->Key("primitives");
	jsonWriter->StartArray();
	for (GLTF::Primitive* primitive : this->primitives) {
		jsonWriter->StartObject();
		primitive->writeJSON(jsonWriter, options);
		jsonWriter->EndObject();
	}
	jsonWriter->EndArray();
	GLTF::Object::writeJSON(writer, options);
}