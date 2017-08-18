//#ifdef USE_DRACO
#include "GLTFDracoExtension.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include <iostream>

void GLTF::DracoExtension::writeJSON(void* writer, GLTF::Options* options) {
  rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;

  // std::cout << "Writing draco extension.\n";

  jsonWriter->Key("bufferView");
  jsonWriter->Int(this->bufferView->id);
  /*
  jsonWriter->Key("indexCount");
  jsonWriter->Int(this->indexCount);
  jsonWriter->Key("vertexCount");
  jsonWriter->Int(this->vertexCount);
  */
  jsonWriter->Key("attributes");
	jsonWriter->StartObject();
	for (const auto& attribute : this->attribute_to_id) {
		jsonWriter->Key(attribute.first.c_str());
		jsonWriter->Int(attribute.second);
	}
	jsonWriter->EndObject();
}

//#endif // USE_DRACO
