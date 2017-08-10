//#ifdef USE_DRACO
#include "GLTFDracoExtension.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include <iostream>

const char* GLTF::DracoAttribute::getTypeName() {
	switch (this->type) {
	case GLTF::Accessor::Type::SCALAR:
		return "SCALAR";
	case GLTF::Accessor::Type::VEC2:
		return "VEC2";
	case GLTF::Accessor::Type::VEC3:
		return "VEC3";
	case GLTF::Accessor::Type::VEC4:
		return "VEC4";
	case GLTF::Accessor::Type::MAT2:
		return "MAT2";
	case GLTF::Accessor::Type::MAT3:
		return "MAT3";
	case GLTF::Accessor::Type::MAT4:
		return "MAT4";
	}
	return "";
}

void GLTF::DracoAttribute::writeJSON(void* writer, GLTF::Options* options) {
  rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;
  jsonWriter->Key("semantic");
  jsonWriter->String(this->semantic.c_str());
}

void GLTF::DracoExtension::writeJSON(void* writer, GLTF::Options* options) {
  rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;

  std::cout << "Writing draco extension.\n";

  jsonWriter->Key("bufferView");
  jsonWriter->Int(this->bufferView->id);
  jsonWriter->Key("indexCount");
  jsonWriter->Int(this->indexCount);
  jsonWriter->Key("vertexCount");
  jsonWriter->Int(this->vertexCount);
  jsonWriter->Key("attributes");
  jsonWriter->StartArray();
  for (const auto& attribute : this->attributes) {
    jsonWriter->StartObject();
    attribute->writeJSON(jsonWriter, options);
    jsonWriter->EndObject();
  }
  jsonWriter->EndArray();
  jsonWriter->Key("version");
  jsonWriter->String(this->version.c_str());
}

//#endif // USE_DRACO
