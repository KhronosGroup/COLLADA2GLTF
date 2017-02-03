#include "GLTFSampler.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

int GLTF::Sampler::INSTANCE_COUNT = 0;

GLTF::Sampler::Sampler() {
	this->id = "texture_" + std::to_string(GLTF::Sampler::INSTANCE_COUNT);
	GLTF::Sampler::INSTANCE_COUNT++;
}

void GLTF::Sampler::writeJSON(void* writer) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;
	jsonWriter->Key("magFilter");
	jsonWriter->Int((int)magFilter);
	jsonWriter->Key("minFilter");
	jsonWriter->Int((int)minFilter);
	jsonWriter->Key("wrapS");
	jsonWriter->Int((int)wrapS);
	jsonWriter->Key("wrapT");
	jsonWriter->Int((int)wrapT);
	GLTF::Object::writeJSON(writer);
}