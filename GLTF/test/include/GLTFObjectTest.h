// Copyright 2020 The Khronos® Group Inc.
#pragma once

#include "GLTFOptions.h"
#include "gtest/gtest.h"

class GLTFObjectTest : public ::testing::Test {
 public:
  GLTF::Options* options;

  GLTFObjectTest();
  ~GLTFObjectTest();
};
