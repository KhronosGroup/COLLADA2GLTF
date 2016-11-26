#include "GLTFAccessor.h"
#include "GLTFAccessorTest.h"

TEST(GLTFAccessorTest, CreateFromData) {
  float points[12] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0};
  unsigned char* data = (unsigned char*)points;
  GLTF::Accessor* accessor = new GLTF::Accessor(GLTF::Accessor::Type::VEC3,
    GLTF::Constants::WebGL::FLOAT,
    (unsigned char*)points,
    12 * sizeof(float),
    GLTF::Constants::WebGL::ARRAY_BUFFER
  );
  double* min = accessor->min;
  ASSERT_TRUE(min != NULL);
  EXPECT_EQ(min[0], 1.0);
  EXPECT_EQ(min[1], 2.0);
  EXPECT_EQ(min[2], 3.0);

  double* max = accessor->max;
  ASSERT_TRUE(max != NULL);
  EXPECT_EQ(max[0], 10.0);
  EXPECT_EQ(max[1], 11.0);
  EXPECT_EQ(max[2], 12.0);

  double component[3];
  for (int i = 0; i < accessor->count; i++) {
    accessor->getComponentAtIndex(i, component);
    EXPECT_EQ(component[0], i * 3 + 1);
    EXPECT_EQ(component[1], i * 3 + 2);
    EXPECT_EQ(component[2], i * 3 + 3);
  }
}
