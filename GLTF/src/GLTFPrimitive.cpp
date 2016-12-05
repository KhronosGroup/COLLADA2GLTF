#include "GLTFPrimitive.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

GLTF::Object* GLTF::Primitive::clone() {
	GLTF::Primitive* clone = (GLTF::Primitive*)GLTF::Object::clone();
	for (const auto& attribute : this->attributes) {
		clone->attributes.insert(attribute);
	}
	clone->indices = this->indices;
	clone->material = this->material;
	clone->mode = this->mode;
	return clone;
}

void GLTF::Primitive::writeJSON(void* writer) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;
	jsonWriter->Key("attributes");
	jsonWriter->StartObject();
	for (const auto& attribute : this->attributes) {
		jsonWriter->Key(attribute.first.c_str());
		jsonWriter->String(attribute.second->id.c_str());
	}
	jsonWriter->EndObject();
	if (this->indices) {
		jsonWriter->Key("indices");
		jsonWriter->String(this->indices->id.c_str());
	}
	jsonWriter->Key("mode");
	jsonWriter->Int((int)this->mode);
	if (this->material) {
		jsonWriter->Key("material");
		jsonWriter->String(this->material->id.c_str());
	}
	GLTF::Object::writeJSON(writer);
}