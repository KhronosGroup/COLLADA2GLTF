#include "GLTFProgram.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

std::string GLTF::Program::typeName() {
	return "program";
}

void GLTF::Program::writeJSON(void* writer, GLTF::Options* options) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;

	jsonWriter->Key("attributes");
	jsonWriter->StartArray();
	for (std::string attribute : attributes) {
		jsonWriter->String(attribute.c_str());
	}
	jsonWriter->EndArray();

	if (fragmentShader != NULL) {
		jsonWriter->Key("fragmentShader");
		if (options->version == "1.0") {
			jsonWriter->String(fragmentShader->getStringId().c_str());
		}
		else {
			jsonWriter->Int(fragmentShader->id);
		}
	}
	if (vertexShader != NULL) {
		jsonWriter->Key("vertexShader");
		if (options->version == "1.0") {
			jsonWriter->String(vertexShader->getStringId().c_str());
		}
		else {
			jsonWriter->Int(vertexShader->id);
		}
	}
	GLTF::Object::writeJSON(writer, options);
}