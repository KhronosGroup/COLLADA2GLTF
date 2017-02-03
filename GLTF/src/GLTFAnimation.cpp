#include "GLTFAnimation.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

int GLTF::Animation::INSTANCE_COUNT = 0;

GLTF::Animation::Animation() {
	this->id = "animation_" + std::to_string(INSTANCE_COUNT);
	INSTANCE_COUNT++;
}

void GLTF::Animation::writeJSON(void* writer) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;

	jsonWriter->Key("channels");
	jsonWriter->StartArray();

	std::map<std::string, GLTF::Animation::Sampler*> samplers;
	for (GLTF::Animation::Channel* channel : channels) {
		samplers[channel->sampler->id] = channel->sampler;
		jsonWriter->StartObject();
		channel->writeJSON(writer);
		jsonWriter->EndObject();
	}
	jsonWriter->EndArray();

	jsonWriter->Key("samplers");
	jsonWriter->StartObject();
	for (auto const& samplerMapEntry : samplers) {
		std::string samplerId = samplerMapEntry.first;
		GLTF::Animation::Sampler* sampler = samplerMapEntry.second;
		jsonWriter->Key(samplerId.c_str());
		jsonWriter->StartObject();
		sampler->writeJSON(writer);
		jsonWriter->EndObject();
	}
	jsonWriter->EndObject();
	samplers.clear();
	GLTF::Object::writeJSON(writer);
}

void GLTF::Animation::Sampler::writeJSON(void* writer) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;

	jsonWriter->Key("input");
	jsonWriter->String(input->id.c_str());
	jsonWriter->Key("interpolation");
	jsonWriter->String(interpolation.c_str());
	jsonWriter->Key("output");
	jsonWriter->String(output->id.c_str());
	GLTF::Object::writeJSON(writer);
}

void GLTF::Animation::Channel::writeJSON(void* writer) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;

	jsonWriter->Key("sampler");
	jsonWriter->String(sampler->id.c_str());
	jsonWriter->Key("target");
	jsonWriter->StartObject();
	target->writeJSON(writer);
	jsonWriter->EndObject();

	GLTF::Object::writeJSON(writer);
}

void GLTF::Animation::Channel::Target::writeJSON(void* writer) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;
	
	jsonWriter->Key("id");
	jsonWriter->String(node->id.c_str());
	jsonWriter->Key("path");
	switch (path) {
	case Path::TRANSLATION:
		jsonWriter->String("translation");
		break;
	case Path::ROTATION:
		jsonWriter->String("rotation");
		break;
	case Path::SCALE:
		jsonWriter->String("scale");
	}

	GLTF::Object::writeJSON(writer);
}