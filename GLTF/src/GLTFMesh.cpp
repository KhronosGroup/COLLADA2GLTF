#include "GLTFMesh.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

GLTF::Object* GLTF::Mesh::clone(GLTF::Object* clone) {
	GLTF::Mesh* mesh = dynamic_cast<GLTF::Mesh*>(clone);
	if (mesh != NULL) {
		for (GLTF::Primitive* primitive : this->primitives) {
			GLTF::Primitive* clonePrimitive = new GLTF::Primitive();
			primitive->clone(clonePrimitive);
			if (clonePrimitive != NULL) {
				mesh->primitives.push_back(clonePrimitive);
			}
		}
	}
	return mesh;
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