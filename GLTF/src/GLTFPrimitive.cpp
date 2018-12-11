#include "GLTFPrimitive.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include <algorithm>

GLTF::Primitive::~Primitive() {
    std::for_each(targets.begin(), targets.end(), std::default_delete<Target>());
}

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
		primitive->targets = std::vector<Target*>();
		for (auto* target : this->targets) {
			primitive->targets.push_back(target);
		}
	}
	Object::clone(clone);
	return primitive;
}

void GLTF::Primitive::writeJSON(void* writer, Options* options) {
	auto* jsonWriter = static_cast<rapidjson::Writer<rapidjson::StringBuffer>*>(writer);
	jsonWriter->Key("attributes");
	jsonWriter->StartObject();
	for (const auto& attribute : this->attributes) {
		jsonWriter->Key(attribute.first.c_str());
		if (options->version == "1.0") {
			jsonWriter->String(attribute.second->getStringId().c_str());
		}
		else {
			jsonWriter->Int(attribute.second->id);
		}
	}
	jsonWriter->EndObject();
	if (this->indices) {
		jsonWriter->Key("indices");
		if (options->version == "1.0") {
			jsonWriter->String(indices->getStringId().c_str());
		}
		else {
			jsonWriter->Int(indices->id);
		}
	}
	jsonWriter->Key("mode");
	jsonWriter->Int(static_cast<int>(this->mode));
	if (this->material) {
		jsonWriter->Key("material");
		if (options->version == "1.0") {
			jsonWriter->String(material->getStringId().c_str());
		}
		else {
			jsonWriter->Int(material->id);
		}
	}
	if (!this->targets.empty()) {
		jsonWriter->Key("targets");
		jsonWriter->StartArray();
		for (auto* target : this->targets) {
			target->writeJSON(writer, options);
		}
		jsonWriter->EndArray();
	}
	Object::writeJSON(writer, options);
}

GLTF::Primitive::Target* GLTF::Primitive::Target::clone(Object* clone) {
	Target* target = dynamic_cast<Target*>(clone);
	if (target != nullptr) {
		target->attributes = std::map<std::string, Accessor*>();
		for (const auto& attribute : this->attributes) {
			target->attributes.insert(attribute);
		}
	}
	return target;
}

void GLTF::Primitive::Target::writeJSON(void* writer, Options* options) {
	auto* jsonWriter = static_cast<rapidjson::Writer<rapidjson::StringBuffer>*>(writer);
	jsonWriter->StartObject();
	for (const auto& attribute : this->attributes) {
		jsonWriter->Key(attribute.first.c_str());
		jsonWriter->Int(attribute.second->id);
	}
	jsonWriter->EndObject();
}
