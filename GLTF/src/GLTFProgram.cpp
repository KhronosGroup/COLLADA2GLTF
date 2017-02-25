#include "GLTFProgram.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

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
		jsonWriter->Int(fragmentShader->id);
	}
	if (vertexShader != NULL) {
		jsonWriter->Key("vertexShader");
		jsonWriter->Int(vertexShader->id);
	}
	GLTF::Object::writeJSON(writer, options);
}