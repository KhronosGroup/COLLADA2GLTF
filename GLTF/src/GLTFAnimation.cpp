#include "GLTFAnimation.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

std::string pathString(GLTF::Animation::Path path) {
	switch (path) {
	case GLTF::Animation::Path::TRANSLATION:
		return "translation";
	case GLTF::Animation::Path::ROTATION:
		return "rotation";
	case GLTF::Animation::Path::SCALE:
		return "scale";
	case GLTF::Animation::Path::WEIGHTS:
		return "weights";
	}
	return "unknown";
}

std::string GLTF::Animation::typeName() {
	return "animation";
}

void GLTF::Animation::writeJSON(void* writer, GLTF::Options* options) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;

	jsonWriter->Key("channels");
	jsonWriter->StartArray();

	std::vector<GLTF::Animation::Sampler*> samplers;
	for (GLTF::Animation::Channel* channel : channels) {
		if (channel->target->node->id >= 0) {
			if (channel->sampler->id < 0) {
				channel->sampler->id = samplers.size();
				channel->sampler->path = channel->target->path;
				samplers.push_back(channel->sampler);
			}
			jsonWriter->StartObject();
			channel->writeJSON(writer, options);
			jsonWriter->EndObject();
		}
	}
	jsonWriter->EndArray();

	std::map<std::string, std::string> parameterMap;
	if (options->version == "1.0") {
		int timeIndex = 0;
		std::map<std::string, std::string>::iterator findParameter;
		std::map<std::string, int> pathCounts;
		std::map<std::string, int>::iterator findPathCount;
		for (GLTF::Animation::Sampler* sampler : samplers) {
			std::string inputString = "TIME";
			std::string inputAccessorId = sampler->input->getStringId();
			if (timeIndex > 0) {
				findParameter = parameterMap.find(inputAccessorId);
				if (findParameter == parameterMap.end()) {
					inputString = "TIME_" + std::to_string(timeIndex);
					parameterMap[inputAccessorId] = inputString;
					timeIndex++;
				}
			}
			else {
				parameterMap[inputAccessorId] = inputString;
				timeIndex++;
			}
			sampler->inputString = inputString;
			std::string path = pathString(sampler->path);
			int count = 0;
			findPathCount = pathCounts.find(path);
			if (findPathCount == pathCounts.end()) {
				pathCounts[path] = 1;
			}
			else {
				count = findPathCount->second;
				pathCounts[path]++;
			}
			std::string outputString = path + (count > 0 ? "_" + std::to_string(count) : "");
			std::string outputAccessorId = sampler->output->getStringId();
			parameterMap[outputAccessorId] = outputString;
			sampler->outputString = outputString;
		}
		jsonWriter->Key("parameters");
		jsonWriter->StartObject();
		for (auto parameter : parameterMap) {
			std::string accessorId = parameter.first;
			std::string parameterName = parameter.second;
			jsonWriter->Key(parameterName.c_str());
			jsonWriter->String(accessorId.c_str());
		}
		jsonWriter->EndObject();
	}

	jsonWriter->Key("samplers");
	if (options->version == "1.0") {
		jsonWriter->StartObject();
	}
	else {
		jsonWriter->StartArray();
	}
	for (GLTF::Animation::Sampler* sampler : samplers) {
		if (options->version == "1.0") {
			jsonWriter->Key(sampler->getStringId().c_str());
		}
		jsonWriter->StartObject();
		sampler->writeJSON(writer, options);
		jsonWriter->EndObject();
	}
	if (options->version == "1.0") {
		jsonWriter->EndObject();
	}
	else {
		jsonWriter->EndArray();
	}
	samplers.clear();
	GLTF::Object::writeJSON(writer, options);
}

std::string GLTF::Animation::Sampler::typeName() {
	return "sampler";
}

void GLTF::Animation::Sampler::writeJSON(void* writer, GLTF::Options* options) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;

	jsonWriter->Key("input");
	if (options->version == "1.0") {
		jsonWriter->String(inputString.c_str());
	}
	else {
		jsonWriter->Int(input->id);
	}
	jsonWriter->Key("interpolation");
	jsonWriter->String(interpolation.c_str());
	jsonWriter->Key("output");
	if (options->version == "1.0") {
		jsonWriter->String(outputString.c_str());
	}
	else {
		jsonWriter->Int(output->id);
	}
	GLTF::Object::writeJSON(writer, options);
}

void GLTF::Animation::Channel::writeJSON(void* writer, GLTF::Options* options) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;

	jsonWriter->Key("sampler");
	if (options->version == "1.0") {
		jsonWriter->String(sampler->getStringId().c_str());
	}
	else {
		jsonWriter->Int(sampler->id);
	}
	jsonWriter->Key("target");
	jsonWriter->StartObject();
	target->writeJSON(writer, options);
	jsonWriter->EndObject();

	GLTF::Object::writeJSON(writer, options);
}

void GLTF::Animation::Channel::Target::writeJSON(void* writer, GLTF::Options* options) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;
	
	if (options->version == "1.0") {
		jsonWriter->Key("id");
		jsonWriter->String(node->getStringId().c_str());
	}
	else {
		jsonWriter->Key("node");
		jsonWriter->Int(node->id);
	}
	jsonWriter->Key("path");
	jsonWriter->String(pathString(path).c_str());

	GLTF::Object::writeJSON(writer, options);
}