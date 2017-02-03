#include "GLTFTexture.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

int GLTF::Texture::INSTANCE_COUNT = 0;

GLTF::Texture::Texture() {
	this->id = "texture_" + std::to_string(GLTF::Texture::INSTANCE_COUNT);
	GLTF::Texture::INSTANCE_COUNT++;
}

void GLTF::Texture::writeJSON(void* writer) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;
	jsonWriter->Key("format");
	jsonWriter->Int((int)format);
	jsonWriter->Key("internalFormat");
	jsonWriter->Int((int)format);
	jsonWriter->Key("sampler");
	jsonWriter->String(sampler->id.c_str());
	jsonWriter->Key("source");
	jsonWriter->String(this->source->id.c_str());
	jsonWriter->Key("target");
	jsonWriter->Int((int)target);
	jsonWriter->Key("type");
	jsonWriter->Int((int)type);
	GLTF::Object::writeJSON(writer);
}