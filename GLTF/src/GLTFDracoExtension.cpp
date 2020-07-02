// Copyright 2020 The Khronos® Group Inc.
#include "GLTFDracoExtension.h"

#include <iostream>

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

void GLTF::DracoExtension::writeJSON(void* writer, GLTF::Options* options) {
  rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter =
      (rapidjson::Writer<rapidjson::StringBuffer>*)writer;
  jsonWriter->Key("bufferView");
  jsonWriter->Int(this->bufferView->id);
  jsonWriter->Key("attributes");
  jsonWriter->StartObject();
  for (const auto& attribute : this->attributeToId) {
    jsonWriter->Key(attribute.first.c_str());
    jsonWriter->Int(attribute.second);
  }
  jsonWriter->EndObject();
}
