#include "GLTFObject.h"
#include "GLTFExtension.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

void GLTF::Object::addExtra(GLTF::Object* extra) {
  this->extras.push_back(extra);
}

void GLTF::Object::addExtension(GLTF::Extension* extension) {
  this->extensions.push_back(extension);
}

GLTF::Object* GLTF::Object::clone() {
	GLTF::Object* clone = new GLTF::Object();
	clone->id = this->id;
	clone->name = this->name;
	for (GLTF::Object* extra : this->extras) {
		clone->extras.push_back(extra->clone());
	}
	for (GLTF::Extension* extension : this->extensions) {
		clone->extensions.push_back((GLTF::Extension*)extension->clone());
	}
	return clone;
}

void GLTF::Object::writeJSON(void* writer) {
  rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;
  if (this->name.length() > 0) {
    jsonWriter->Key("name");
    jsonWriter->String(this->name.c_str());
  }
  if (this->extensions.size() > 0) {
    jsonWriter->Key("extensions");
    jsonWriter->StartObject();
    for (GLTF::Extension* extension : this->extensions) {
      jsonWriter->Key(extension->id.c_str());
      jsonWriter->StartObject();
      extension->writeJSON(writer);
      jsonWriter->EndObject();
    }
    jsonWriter->EndObject();
  }
  if (this->extras.size() > 0) {
    jsonWriter->Key("extras");
    jsonWriter->StartObject();
    for (GLTF::Object* extra : this->extras) {
      jsonWriter->Key(extra->id.c_str());
      jsonWriter->StartObject();
      extra->writeJSON(writer);
      jsonWriter->EndObject();
    }
    jsonWriter->EndObject();
  }
}
