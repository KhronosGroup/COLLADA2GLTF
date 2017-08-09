#include "GLTFTexture.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

void GLTF::Texture::writeJSON(void* writer, GLTF::Options* options) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;
	jsonWriter->Key("sampler");
	jsonWriter->Int(sampler->id);
	jsonWriter->Key("source");
	jsonWriter->Int(this->source->id);
	GLTF::Object::writeJSON(writer, options);
}