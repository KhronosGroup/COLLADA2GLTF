#include "GLTFObject.h"
#include "GLTFExtension.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

void GLTF::Object::addExtra(GLTF::Object* extra) {
  if (!this->extras) {
    this->extras = new std::vector<GLTF::Object*>();
  }
  this->extras->push_back(extra);
}

void GLTF::Object::addExtension(GLTF::Extension* extension) {
  if (!this->extensions) {
    this->extensions = new std::vector<GLTF::Extension*>();
  }
  this->extensions->push_back(extension);
}

void GLTF::Object::writeJSON(void* writer) {
  rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;
  if (this->name.length() > 0) {
    jsonWriter->Key("name");
    jsonWriter->String(this->name.c_str());
  }
  if (this->extensions) {
    jsonWriter->Key("extensions");
    jsonWriter->StartObject();
    for (GLTF::Extension* extension : *this->extensions) {
      jsonWriter->Key(extension->id.c_str());
      jsonWriter->StartObject();
      extension->writeJSON(writer);
      jsonWriter->EndObject();
    }
    jsonWriter->EndObject();
  }
  if (this->extras) {
    jsonWriter->Key("extras");
    jsonWriter->StartObject();
    for (GLTF::Object* extra : *this->extras) {
      jsonWriter->Key(extra->id.c_str());
      jsonWriter->StartObject();
      extra->writeJSON(writer);
      jsonWriter->EndObject();
    }
    jsonWriter->EndObject();
  }
}
