// Copyright 2020 The Khronos® Group Inc.
#include "GLTFAccessorTest.h"

#include "GLTFAccessor.h"

TEST(GLTFAccessorTest, CreateFromData) {
  float points[12] = {1.0, 2.0, 3.0, 4.0,  5.0,  6.0,
                      7.0, 8.0, 9.0, 10.0, 11.0, 12.0};
  GLTF::Accessor* accessor = new GLTF::Accessor(
      GLTF::Accessor::Type::VEC3, GLTF::Constants::WebGL::FLOAT,
      (unsigned char*)points, 4, GLTF::Constants::WebGL::ARRAY_BUFFER);
  float* min = accessor->min;
  ASSERT_TRUE(min != NULL);
  EXPECT_EQ(min[0], 1.0);
  EXPECT_EQ(min[1], 2.0);
  EXPECT_EQ(min[2], 3.0);

  float* max = accessor->max;
  ASSERT_TRUE(max != NULL);
  EXPECT_EQ(max[0], 10.0);
  EXPECT_EQ(max[1], 11.0);
  EXPECT_EQ(max[2], 12.0);

  float component[3];
  for (int i = 0; i < accessor->count; i++) {
    accessor->getComponentAtIndex(i, component);
    EXPECT_EQ(component[0], i * 3 + 1);
    EXPECT_EQ(component[1], i * 3 + 2);
    EXPECT_EQ(component[2], i * 3 + 3);
  }
  delete accessor;
}

TEST(GLTFAccessorTest, CreateOnBuffer) {
  float pointsBuffer[12] = {1.0, 2.0, 3.0, 4.0,  5.0,  6.0,
                            7.0, 8.0, 9.0, 10.0, 11.0, 12.0};
  GLTF::Accessor* accessorFromData = new GLTF::Accessor(
      GLTF::Accessor::Type::VEC3, GLTF::Constants::WebGL::FLOAT,
      (unsigned char*)pointsBuffer, 4, GLTF::Constants::WebGL::ARRAY_BUFFER);
  GLTF::BufferView* bufferView = accessorFromData->bufferView;
  float points[6] = {13.0, 14.0, 15.0, 16.0, 17.0, 18.0};
  GLTF::Accessor* accessor = new GLTF::Accessor(
      GLTF::Accessor::Type::VEC3, GLTF::Constants::WebGL::FLOAT,
      (unsigned char*)points, 2, bufferView);

  float* min = accessor->min;
  ASSERT_TRUE(min != NULL);
  EXPECT_EQ(min[0], 13.0);
  EXPECT_EQ(min[1], 14.0);
  EXPECT_EQ(min[2], 15.0);

  float* max = accessor->max;
  ASSERT_TRUE(max != NULL);
  EXPECT_EQ(max[0], 16.0);
  EXPECT_EQ(max[1], 17.0);
  EXPECT_EQ(max[2], 18.0);

  float component[3];
  for (int i = 0; i < accessor->count; i++) {
    accessor->getComponentAtIndex(i, component);
    EXPECT_EQ(component[0], (i + 4) * 3 + 1);
    EXPECT_EQ(component[1], (i + 4) * 3 + 2);
    EXPECT_EQ(component[2], (i + 4) * 3 + 3);
  }
}
