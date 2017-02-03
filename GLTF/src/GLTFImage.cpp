#include "Base64.h"
#include "GLTFImage.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

GLTF::Image::Image(std::string uri) {
	this->uri = uri;
}

GLTF::Image::Image(unsigned char* data, size_t length, std::string mimeType) {
	this->uri = "data:" + mimeType + ";base64," + GLTF::Base64::encode(data, length);
}

void GLTF::Image::writeJSON(void* writer) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;
	jsonWriter->Key("uri");
	jsonWriter->String(this->uri.c_str());
	GLTF::Object::writeJSON(writer);
}