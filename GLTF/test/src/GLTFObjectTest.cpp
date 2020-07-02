// Copyright 2020 The Khronos® Group Inc.
#include "GLTFObjectTest.h"

#include "GLTFExtension.h"
#include "GLTFObject.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

GLTFObjectTest::GLTFObjectTest() { options = new GLTF::Options(); }

GLTFObjectTest::~GLTFObjectTest() { delete options; }

rapidjson::StringBuffer writeObject(GLTF::Object* object,
                                    GLTF::Options* options) {
  rapidjson::StringBuffer s;
  rapidjson::Writer<rapidjson::StringBuffer> writer(s);
  writer.StartObject();
  object->writeJSON(&writer, options);
  writer.EndObject();
  return s;
}

TEST_F(GLTFObjectTest, WriteJSON_NoParameters) {
  GLTF::Object* object = new GLTF::Object();
  rapidjson::StringBuffer s = writeObject(object, this->options);

  EXPECT_STREQ(s.GetString(), "{}");

  free(object);
}

TEST_F(GLTFObjectTest, WriteJSON_WithName) {
  GLTF::Object* object = new GLTF::Object();
  object->name = "test";
  rapidjson::StringBuffer s = writeObject(object, this->options);

  EXPECT_STREQ(s.GetString(), "{\"name\":\"test\"}");

  free(object);
}

TEST_F(GLTFObjectTest, WriteJSON_WithExtra) {
  GLTF::Object* object = new GLTF::Object();
  GLTF::Object* extra = new GLTF::Object();
  extra->name = "extra,extra";
  object->extras["extra"] = extra;
  rapidjson::StringBuffer s = writeObject(object, this->options);

  EXPECT_STREQ(s.GetString(),
               "{\"extras\":{\"extra\":{\"name\":\"extra,extra\"}}}");

  free(object);
}

TEST_F(GLTFObjectTest, WriteJSON_WithExtension) {
  GLTF::Object* object = new GLTF::Object();
  GLTF::Extension* extension = new GLTF::Extension();
  object->extensions["KHR_materials_common"] = extension;
  rapidjson::StringBuffer s = writeObject(object, this->options);

  EXPECT_STREQ(s.GetString(), "{\"extensions\":{\"KHR_materials_common\":{}}}");

  free(object);
}
