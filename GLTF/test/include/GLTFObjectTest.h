#pragma once

#include "GLTFOptions.h"
#include "gtest/gtest.h"

namespace {
class GLTFObjectTest : public ::testing::Test {
 public:
  GLTF::Options* options;

  GLTFObjectTest();
  ~GLTFObjectTest();
};
}  // namespace
