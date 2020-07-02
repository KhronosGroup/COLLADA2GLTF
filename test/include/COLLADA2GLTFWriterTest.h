// Copyright 2020 The Khronos® Group Inc.
#pragma once

#include "COLLADA2GLTFWriter.h"
#include "gtest/gtest.h"

class COLLADA2GLTFWriterTest : public ::testing::Test {
 public:
  COLLADA2GLTF::Writer* writer;
  COLLADA2GLTF::Options* options;
  COLLADA2GLTF::ExtrasHandler* extrasHandler;
  GLTF::Asset* asset;

  COLLADA2GLTFWriterTest();
  ~COLLADA2GLTFWriterTest();
};
