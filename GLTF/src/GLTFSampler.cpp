// Copyright 2020 The Khronos® Group Inc.
#include "GLTFSampler.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

std::string GLTF::Sampler::typeName() { return "sampler"; }

void GLTF::Sampler::writeJSON(void* writer, GLTF::Options* options) {
  rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter =
      (rapidjson::Writer<rapidjson::StringBuffer>*)writer;
  jsonWriter->Key("magFilter");
  jsonWriter->Int(static_cast<int>(magFilter));
  jsonWriter->Key("minFilter");
  jsonWriter->Int(static_cast<int>(minFilter));
  jsonWriter->Key("wrapS");
  jsonWriter->Int(static_cast<int>(wrapS));
  jsonWriter->Key("wrapT");
  jsonWriter->Int(static_cast<int>(wrapT));
  GLTF::Object::writeJSON(writer, options);
}
