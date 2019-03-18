#include "GLTFTechnique.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

GLTF::Technique::~Technique() {
    for (auto& kv : parameters) {
        delete kv.second;
    }
}

std::string GLTF::Technique::typeName() {
	return "technique";
}

void GLTF::Technique::writeJSON(void* writer, GLTF::Options* options) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;

	jsonWriter->Key("attributes");
	jsonWriter->StartObject();
	for (auto attribute : attributes) {
		jsonWriter->Key(attribute.first.c_str());
		jsonWriter->String(attribute.second.c_str());
	}
	jsonWriter->EndObject();

	jsonWriter->Key("parameters");
	jsonWriter->StartObject();
	for (auto parameter : parameters) {
		jsonWriter->Key(parameter.first.c_str());
		jsonWriter->StartObject();
		GLTF::Technique::Parameter* parameterValue = parameter.second;
		std::string semantic = parameterValue->semantic;
		if (semantic != "") {
			jsonWriter->Key("semantic");
			jsonWriter->String(semantic.c_str());
		}
		if (options->version == "1.0") {
			if (parameterValue->nodeString != "") {
				jsonWriter->Key("node");
				jsonWriter->String(parameterValue->nodeString.c_str());
			}
		}
		else {
			if (parameterValue->node >= 0) {
				jsonWriter->Key("node");
				jsonWriter->Int(parameterValue->node);
			}
		}
		if (parameterValue->value != NULL) {
			jsonWriter->Key("value");
			jsonWriter->StartArray();
			for (int i = 0; i < parameterValue->valueLength; i++) {
				jsonWriter->Double(parameterValue->value[i]);
			}
			jsonWriter->EndArray();
		}
		if (parameterValue->count >= 0) {
			jsonWriter->Key("count");
			jsonWriter->Int(parameterValue->count);
		}
		jsonWriter->Key("type");
		jsonWriter->Int((int)parameterValue->type);
		jsonWriter->EndObject();
	}
	jsonWriter->EndObject();

	if (program != NULL) {
		jsonWriter->Key("program");
		if (options->version == "1.0") {
			jsonWriter->String(program->getStringId().c_str());
		}
		else {
			jsonWriter->Int(program->id);
		}
	}

	if (enableStates.size() > 0 || depthMask != NULL || blendEquationSeparate.size() > 0 || blendFuncSeparate.size() > 0) {
		jsonWriter->Key("states");
		jsonWriter->StartObject();
		if (enableStates.size() > 0) {
			jsonWriter->Key("enable");
			jsonWriter->StartArray();
			for (GLTF::Constants::WebGL constant : enableStates) {
				jsonWriter->Int((int)constant);
			}
			jsonWriter->EndArray();
		}
		if (depthMask != NULL) {
			jsonWriter->Key("depthMask");
			jsonWriter->Bool(depthMask[0]);
		}
		if (blendEquationSeparate.size() > 0 || blendFuncSeparate.size() > 0) {
			jsonWriter->Key("functions");
			jsonWriter->StartObject();
			if (blendEquationSeparate.size() > 0) {
				jsonWriter->Key("blendEquationSeparate");
				jsonWriter->StartArray();
				for (GLTF::Constants::WebGL constant : blendEquationSeparate) {
					jsonWriter->Int((int)constant);
				}
				jsonWriter->EndArray();
			}
			if (blendFuncSeparate.size() > 0) {
				jsonWriter->Key("blendFuncSeparate");
				jsonWriter->StartArray();
				for (GLTF::Constants::WebGL constant : blendFuncSeparate) {
					jsonWriter->Int((int)constant);
				}
				jsonWriter->EndArray();
			}
			jsonWriter->EndObject();
		}
		jsonWriter->EndObject();
	}

	if (uniforms.size() > 0) {
		jsonWriter->Key("uniforms");
		jsonWriter->StartObject();
		for (auto uniform : uniforms) {
			jsonWriter->Key(uniform.first.c_str());
			jsonWriter->String(uniform.second.c_str());
		}
		jsonWriter->EndObject();
	}
	GLTF::Object::writeJSON(writer, options);
}