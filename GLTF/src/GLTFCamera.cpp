// Copyright 2020 The Khronos® Group Inc.
#include "GLTFCamera.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

std::string GLTF::Camera::typeName() { return "camera"; }

void GLTF::Camera::writeJSON(void* writer, GLTF::Options* options) {
  rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter =
      (rapidjson::Writer<rapidjson::StringBuffer>*)writer;

  if (type != Type::UNKNOWN) {
    jsonWriter->Key("type");
    if (type == Type::PERSPECTIVE) {
      jsonWriter->String("perspective");
    } else if (type == Type::ORTHOGRAPHIC) {
      jsonWriter->String("orthographic");
    }
  }
  GLTF::Object::writeJSON(writer, options);
}

void GLTF::CameraOrthographic::writeJSON(void* writer, GLTF::Options* options) {
  rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter =
      (rapidjson::Writer<rapidjson::StringBuffer>*)writer;

  jsonWriter->Key("orthographic");
  jsonWriter->StartObject();
  jsonWriter->Key("xmag");
  jsonWriter->Double(xmag);
  jsonWriter->Key("ymag");
  jsonWriter->Double(ymag);
  jsonWriter->Key("zfar");
  jsonWriter->Double(zfar);
  jsonWriter->Key("znear");
  jsonWriter->Double(znear);
  jsonWriter->EndObject();

  GLTF::Camera::writeJSON(writer, options);
}

void GLTF::CameraPerspective::writeJSON(void* writer, GLTF::Options* options) {
  rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter =
      (rapidjson::Writer<rapidjson::StringBuffer>*)writer;

  jsonWriter->Key("perspective");
  jsonWriter->StartObject();
  if (aspectRatio > 0) {
    jsonWriter->Key("aspectRatio");
    jsonWriter->Double(aspectRatio);
  }
  jsonWriter->Key("yfov");
  jsonWriter->Double(yfov);
  jsonWriter->Key("zfar");
  jsonWriter->Double(zfar);
  jsonWriter->Key("znear");
  jsonWriter->Double(znear);
  jsonWriter->EndObject();

  GLTF::Camera::writeJSON(writer, options);
}
