#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "GLTFExtension.h"
#include "GLTFObject.h"
#include "GLTFObjectTest.h"

rapidjson::StringBuffer writeObject(GLTF::Object* object) {
  rapidjson::StringBuffer s;
  rapidjson::Writer<rapidjson::StringBuffer> writer(s);
  writer.StartObject();
  object->writeJSON(&writer);
  writer.EndObject();
  return s;
}

TEST(GLTFObjectTest, WriteJSON_NoParameters) {
  GLTF::Object* object = new GLTF::Object();
  rapidjson::StringBuffer s = writeObject(object);

  EXPECT_STREQ(s.GetString(), "{}");

  free(object);
}

TEST(GLTFObjectTest, WriteJSON_WithName) {
  GLTF::Object* object = new GLTF::Object();
  object->name = "test";
  rapidjson::StringBuffer s = writeObject(object);

  EXPECT_STREQ(s.GetString(), "{\"name\":\"test\"}");

  free(object);
}

TEST(GLTFObjectTest, WriteJSON_WithExtra) {
  GLTF::Object* object = new GLTF::Object();
  GLTF::Object* extra = new GLTF::Object();
  extra->id = "extra";
  extra->name = "extra,extra";
  object->addExtra(extra);
  rapidjson::StringBuffer s = writeObject(object);

  EXPECT_STREQ(s.GetString(), "{\"extras\":{\"extra\":{\"name\":\"extra,extra\"}}}");

  free(object);
}

TEST(GLTFObjectTest, WriteJSON_WithExtension) {
  GLTF::Object* object = new GLTF::Object();
  GLTF::Extension* extension = new GLTF::Extension();
  extension->id = "KHR_materials_common";
  object->addExtension(extension);
  rapidjson::StringBuffer s = writeObject(object);

  EXPECT_STREQ(s.GetString(), "{\"extensions\":{\"KHR_materials_common\":{}}}");

  free(object);
}
