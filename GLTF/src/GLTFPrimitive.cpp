#include "GLTFPrimitive.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

GLTF::Object* GLTF::Primitive::clone(GLTF::Object* clone) {
	GLTF::Primitive* primitive = dynamic_cast<GLTF::Primitive*>(clone);
	if (primitive != NULL) {
		primitive->attributes = std::map<std::string, GLTF::Accessor*>();
		for (const auto& attribute : this->attributes) {
			primitive->attributes.insert(attribute);
		}
		primitive->indices = this->indices;
		primitive->material = this->material;
		primitive->mode = this->mode;
	}
	return primitive;
}

void GLTF::Primitive::writeJSON(void* writer, GLTF::Options* options) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;
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