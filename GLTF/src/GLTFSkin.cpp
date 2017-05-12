#include "GLTFSkin.h"
#include "GLTFNode.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

void GLTF::Skin::writeJSON(void* writer, GLTF::Options* options) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;

	if (inverseBindMatrices != NULL) {
		jsonWriter->Key("inverseBindMatrices");
		jsonWriter->Int(inverseBindMatrices->id);
	}
	if (skeleton != NULL) {
		jsonWriter->Key("skeleton");
		jsonWriter->Int(skeleton->id);
	}
	jsonWriter->Key("joints");
	jsonWriter->StartArray();
	for (GLTF::Node* node : joints) {
		if (node != NULL) {
			jsonWriter->Int(node->id);
		}
	}
	jsonWriter->EndArray();
	GLTF::Object::writeJSON(writer, options);
}