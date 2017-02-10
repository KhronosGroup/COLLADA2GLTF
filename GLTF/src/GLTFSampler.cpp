#include "GLTFSampler.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

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