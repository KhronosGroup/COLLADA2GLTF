#include "GLTFAnimation.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

void GLTF::Animation::writeJSON(void* writer, GLTF::Options* options) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;

	jsonWriter->Key("channels");
	jsonWriter->StartArray();

	std::vector<GLTF::Animation::Sampler*> samplers;
	for (GLTF::Animation::Channel* channel : channels) {
		if (channel->target->node->id >= 0) {
			if (channel->sampler->id < 0) {
				channel->sampler->id = samplers.size();
				samplers.push_back(channel->sampler);
			}
			jsonWriter->StartObject();
			channel->writeJSON(writer, options);
			jsonWriter->EndObject();
		}
	}
	jsonWriter->EndArray();

	jsonWriter->Key("samplers");
	jsonWriter->StartArray();
	for (GLTF::Animation::Sampler* sampler : samplers) {
		jsonWriter->StartObject();
		sampler->writeJSON(writer, options);
		jsonWriter->EndObject();
	}
	jsonWriter->EndArray();
	samplers.clear();
	GLTF::Object::writeJSON(writer, options);
}

void GLTF::Animation::Sampler::writeJSON(void* writer, GLTF::Options* options) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;

	jsonWriter->Key("input");
	jsonWriter->Int(input->id);
	jsonWriter->Key("interpolation");
	jsonWriter->String(interpolation.c_str());
	jsonWriter->Key("output");
	jsonWriter->Int(output->id);
	GLTF::Object::writeJSON(writer, options);
}

void GLTF::Animation::Channel::writeJSON(void* writer, GLTF::Options* options) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;

	jsonWriter->Key("sampler");
	jsonWriter->Int(sampler->id);
	jsonWriter->Key("target");
	jsonWriter->StartObject();
	target->writeJSON(writer, options);
	jsonWriter->EndObject();

	GLTF::Object::writeJSON(writer, options);
}

void GLTF::Animation::Channel::Target::writeJSON(void* writer, GLTF::Options* options) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;
	
	jsonWriter->Key("id");

	if (node->id < 0) {
		std::string pointBreak = "break point";
	}

	jsonWriter->Int(node->id);
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

	GLTF::Object::writeJSON(writer, options);
}