#include "GLTFTexture.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

void GLTF::Texture::writeJSON(void* writer, GLTF::Options* options) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;
	jsonWriter->Key("format");
	jsonWriter->Int((int)format);
	jsonWriter->Key("internalFormat");
	jsonWriter->Int((int)format);
	jsonWriter->Key("sampler");
	jsonWriter->Int(sampler->id);
	jsonWriter->Key("source");
	jsonWriter->Int(this->source->id);
	jsonWriter->Key("target");
	jsonWriter->Int((int)target);
	jsonWriter->Key("type");
	jsonWriter->Int((int)type);
	GLTF::Object::writeJSON(writer, options);
}