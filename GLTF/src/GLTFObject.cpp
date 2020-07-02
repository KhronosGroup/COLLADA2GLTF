// Copyright 2020 The Khronos® Group Inc.
#include "GLTFObject.h"

#include "GLTFExtension.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

GLTF::Object::~Object() {
  for (auto& kv : extensions) {
    delete kv.second;
  }
  for (auto& kv : extras) {
    delete kv.second;
  }
}

std::string GLTF::Object::getStringId() {
  if (stringId == "") {
    return typeName() + "_" + std::to_string(id);
  }
  return stringId;
}

std::string GLTF::Object::typeName() { return "object"; }

GLTF::Object* GLTF::Object::clone(GLTF::Object* clone) {
  clone->id = this->id;
  clone->name = this->name;
  for (const auto extra : this->extras) {
    clone->extras[extra.first] = extra.second;
  }
  for (const auto extension : this->extensions) {
    clone->extensions[extension.first] = extension.second;
  }
  return clone;
}

void GLTF::Object::writeJSON(void* writer, GLTF::Options* options) {
  rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter =
      (rapidjson::Writer<rapidjson::StringBuffer>*)writer;
  if (this->name.length() > 0) {
    jsonWriter->Key("name");
    jsonWriter->String(this->name.c_str());
  }
  if (this->extensions.size() > 0) {
    jsonWriter->Key("extensions");
    jsonWriter->StartObject();
    for (const auto extension : this->extensions) {
      jsonWriter->Key(extension.first.c_str());
      jsonWriter->StartObject();
      extension.second->writeJSON(writer, options);
      jsonWriter->EndObject();
    }
    jsonWriter->EndObject();
  }
  if (this->extras.size() > 0) {
    jsonWriter->Key("extras");
    jsonWriter->StartObject();
    for (const auto extra : this->extras) {
      jsonWriter->Key(extra.first.c_str());
      jsonWriter->StartObject();
      extra.second->writeJSON(writer, options);
      jsonWriter->EndObject();
    }
    jsonWriter->EndObject();
  }
}
