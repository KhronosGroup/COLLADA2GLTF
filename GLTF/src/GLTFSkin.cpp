#include "GLTFSkin.h"
#include "GLTFNode.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

std::string GLTF::Skin::typeName() {
	return "skin";
}

void GLTF::Skin::writeJSON(void* writer, GLTF::Options* options) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;

	if (inverseBindMatrices != NULL) {
		jsonWriter->Key("inverseBindMatrices");
		if (options->version == "1.0") {
			jsonWriter->String(inverseBindMatrices->getStringId().c_str());
		}
		else {
			jsonWriter->Int(inverseBindMatrices->id);
		}
	}
	if (options->version != "1.0" && skeleton != NULL) {
		jsonWriter->Key("skeleton");
		jsonWriter->Int(skeleton->id);
	}
	if (options->version == "1.0") {
		jsonWriter->Key("jointNames");
	}
	else {
		jsonWriter->Key("joints");
	}
	jsonWriter->StartArray();
	for (GLTF::Node* node : joints) {
		if (node != NULL) {
			if (options->version == "1.0") {
				jsonWriter->String(node->jointName.c_str());
			}
			else {
				jsonWriter->Int(node->id);
			}
		}
	}
	jsonWriter->EndArray();
	GLTF::Object::writeJSON(writer, options);
}