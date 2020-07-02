// Copyright 2020 The Khronos® Group Inc.
#pragma once

#include <string>

namespace Base64 {
char* encode(unsigned char* data, size_t length);
std::string decode(std::string uri);
}  // namespace Base64
