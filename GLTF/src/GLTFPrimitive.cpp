#include "GLTFPrimitive.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include <iostream>

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

void GLTF::Primitive::writeJSON(void* writer, GLTF::Options* options) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;

  std::cout << "Writing primitive.\n";

	jsonWriter->Key("attributes");
	jsonWriter->StartObject();
	for (const auto& attribute : this->attributes) {
		jsonWriter->Key(attribute.first.c_str());
		jsonWriter->Int(attribute.second->id);
	}
	jsonWriter->EndObject();
	if (this->indices) {
		jsonWriter->Key("indices");
		jsonWriter->Int(this->indices->id);
	}
	jsonWriter->Key("mode");
	jsonWriter->Int((int)this->mode);
	if (this->material) {
		jsonWriter->Key("material");
		jsonWriter->Int(this->material->id);
	}
	GLTF::Object::writeJSON(writer, options);
}
