#include "GLTFImage.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

void GLTF::Image::writeJSON(void* writer) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;
	jsonWriter->Key("uri");
	jsonWriter->String(this->uri.c_str());
	GLTF::Object::writeJSON(writer);
}