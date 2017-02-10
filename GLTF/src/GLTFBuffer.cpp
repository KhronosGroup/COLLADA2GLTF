#include "GLTFBuffer.h"
#include "Base64.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

GLTF::Buffer::Buffer(unsigned char* data, int dataLength) {
	this->data = data;
	this->byteLength = dataLength;
}

void GLTF::Buffer::writeJSON(void* writer, GLTF::Options* options) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;
	jsonWriter->Key("byteLength");
	jsonWriter->Int(this->byteLength);
	if (!options->binary) {
		jsonWriter->Key("uri");
		if (options->embeddedBuffers) {
			uri = "data:application/octet-stream;base64," + std::string(Base64::encode(this->data, this->byteLength));
		}
		else {
			uri = std::to_string(id) + ".bin";
		}
		jsonWriter->String(uri.c_str());
	}
	jsonWriter->Key("type");
	jsonWriter->String("arraybuffer");
	GLTF::Object::writeJSON(writer, options);
}
