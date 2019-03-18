#include "GLTFBuffer.h"
#include "Base64.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

GLTF::Buffer::Buffer(unsigned char* data, int dataLength) {
	this->data = data;
	this->byteLength = dataLength;
}

GLTF::Buffer::~Buffer() {
    free(this->data);
}

std::string GLTF::Buffer::typeName() {
	return "buffer";
}

void GLTF::Buffer::writeJSON(void* writer, GLTF::Options* options) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;
	jsonWriter->Key("byteLength");
	jsonWriter->Int(this->byteLength);
	if (!options->binary || !options->embeddedBuffers) {
		jsonWriter->Key("uri");
		if (options->embeddedBuffers) {
			uri = "data:application/octet-stream;base64," + std::string(Base64::encode(this->data, this->byteLength));
		}
		else {
			uri = options->name + std::to_string(id) + ".bin";
		}
		jsonWriter->String(uri.c_str());
	}
	GLTF::Object::writeJSON(writer, options);
}
