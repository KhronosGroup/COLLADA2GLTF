#include "GLTFMaterial.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

void GLTF::Material::Values::writeJSON(void* writer) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;
	jsonWriter->Key("ambient");
	jsonWriter->StartArray();
	for (float val : this->ambient) {
		jsonWriter->Double(val);
	}
	jsonWriter->EndArray();

	jsonWriter->Key("diffuse");
	if (diffuseTexture) {
		jsonWriter->String(diffuseTexture->id.c_str());
	}
	else {
		jsonWriter->StartArray();
		for (float val : this->diffuse) {
			jsonWriter->Double(val);
		}
		jsonWriter->EndArray();
	}

	jsonWriter->Key("emission");
	jsonWriter->StartArray();
	for (float val : this->emission) {
		jsonWriter->Double(val);
	}
	jsonWriter->EndArray();

	jsonWriter->Key("shininess");
	jsonWriter->StartArray();
	jsonWriter->Double(this->shininess[0]);
	jsonWriter->EndArray();

	jsonWriter->Key("specular");
	jsonWriter->StartArray();
	for (float val : this->specular) {
		jsonWriter->Double(val);
	}
	jsonWriter->EndArray();
}

void GLTF::Material::writeJSON(void* writer) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;
	if (this->values) {
		jsonWriter->Key("values");
		jsonWriter->StartObject();
		this->values->writeJSON(writer);
		jsonWriter->EndObject();
	}
	if (this->technique) {
		jsonWriter->Key("technique");
		jsonWriter->String(this->technique->id.c_str());
	}
	GLTF::Object::writeJSON(writer);
}

void GLTF::MaterialCommon::writeJSON(void* writer) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;
	jsonWriter->Key("extensions");
	jsonWriter->StartObject();
	jsonWriter->Key("KHR_materials_common");
	jsonWriter->StartObject();
	jsonWriter->Key("doubleSided");
	jsonWriter->Bool(this->doubleSided);
	if (this->jointCount > 0) {
		jsonWriter->Key("jointCount");
		jsonWriter->Int(this->jointCount);
	}
	jsonWriter->Key("technique");
	jsonWriter->String(this->getTechniqueName());
	jsonWriter->Key("transparent");
	jsonWriter->Bool(this->transparent);
	GLTF::Material::writeJSON(writer);
	jsonWriter->EndObject();
	jsonWriter->EndObject();
	GLTF::Object::writeJSON(writer);
}

const char* GLTF::MaterialCommon::getTechniqueName() {
	switch (this->technique) {
	case BLINN:
		return "BLINN";
	case PHONG:
		return "PHONG";
	case LAMBERT:
		return "LAMBERT";
	case CONSTANT:
		return "CONSTANT";
	}
	return NULL;
}