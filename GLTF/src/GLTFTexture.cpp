// Copyright 2020 The Khronos® Group Inc.
#include "GLTFTexture.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

std::string GLTF::Texture::typeName() { return "texture"; }

void GLTF::Texture::writeJSON(void* writer, GLTF::Options* options) {
  rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter =
      (rapidjson::Writer<rapidjson::StringBuffer>*)writer;
  if (options->version == "1.0") {
    jsonWriter->Key("format");
    jsonWriter->Int(static_cast<int>(GLTF::Constants::WebGL::RGBA));
    jsonWriter->Key("internalFormat");
    jsonWriter->Int(static_cast<int>(GLTF::Constants::WebGL::RGBA));
    jsonWriter->Key("target");
    jsonWriter->Int(static_cast<int>(GLTF::Constants::WebGL::TEXTURE_2D));
    jsonWriter->Key("type");
    jsonWriter->Int(static_cast<int>(GLTF::Constants::WebGL::UNSIGNED_BYTE));
  }
  jsonWriter->Key("sampler");
  if (options->version == "1.0") {
    jsonWriter->String(sampler->getStringId().c_str());
  } else {
    jsonWriter->Int(sampler->id);
  }
  jsonWriter->Key("source");
  if (options->version == "1.0") {
    jsonWriter->String(source->getStringId().c_str());
  } else {
    jsonWriter->Int(source->id);
  }
  GLTF::Object::writeJSON(writer, options);
}
