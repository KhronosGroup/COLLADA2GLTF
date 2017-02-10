#include "GLTFSkin.h"
#include "GLTFNode.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

GLTF::Skin::Skin() {
	bindShapeMatrix = new float[16];
}

void GLTF::Skin::writeJSON(void* writer, GLTF::Options* options) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;

	if (bindShapeMatrix != NULL) {
		jsonWriter->Key("bindShapeMatrix");
		jsonWriter->StartArray();
		for (int i = 0; i < 16; i++) {
			jsonWriter->Double(bindShapeMatrix[i]);
		}
		jsonWriter->EndArray();
	}
	if (inverseBindMatrices != NULL) {
		jsonWriter->Key("inverseBindMatrices");
		jsonWriter->Int(inverseBindMatrices->id);
	}
	jsonWriter->Key("jointNames");
	jsonWriter->StartArray();
	for (GLTF::Node* node : joints) {
		if (node != NULL) {
			jsonWriter->String(node->jointName.c_str());
		}
	}
	jsonWriter->EndArray();
	GLTF::Object::writeJSON(writer, options);
}