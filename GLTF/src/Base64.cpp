// Copyright 2020 The Khronos® Group Inc.
#include "Base64.h"

#include <cmath>

static inline bool is_base64(unsigned char c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}

static const char base64CharSet[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
char* Base64::encode(unsigned char* data, size_t length) {
  size_t base64Length = (size_t)(ceil(length / 3.0) * 4) + 1;
  char* base64 = new char[base64Length];
  int index;
  int j = 0;
  for (size_t i = 0; i < length; i += 3) {
    index = (data[i] & 0xFC) >> 2;
    base64[j] = base64CharSet[index];
    j++;
    index = (data[i] & 0x03) << 4;
    if (i + 1 < length) {
      index |= (data[i + 1] & 0xF0) >> 4;
      base64[j] = base64CharSet[index];
      j++;
      index = (data[i + 1] & 0x0F) << 2;
      if (i + 2 < length) {
        index |= (data[i + 2] & 0xC0) >> 6;
        base64[j] = base64CharSet[index];
        j++;
        index = data[i + 2] & 0x3F;
        base64[j] = base64CharSet[index];
        j++;
      } else {
        base64[j] = base64CharSet[index];
        base64[j + 1] = '=';
      }
    } else {
      base64[j] = base64CharSet[index];
      base64[j + 1] = '=';
      base64[j + 2] = '=';
    }
  }
  base64[base64Length - 1] = '\0';
  return base64;
}

std::string Base64::decode(std::string string) {
  size_t length = string.size();
  size_t i = 0;
  size_t j = 0;
  int in_ = 0;
  unsigned char char_array_4[4], char_array_3[3];
  std::string ret;

  while (length-- && (string[in_] != '=') && is_base64(string[in_])) {
    char_array_4[i++] = string[in_];
    in_++;
    if (i == 4) {
      for (i = 0; i < 4; i++) {
        char_array_4[i] = static_cast<unsigned char>(
            std::string(base64CharSet).find(char_array_4[i]));
      }

      char_array_3[0] =
          (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] =
          ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

      for (i = 0; i < 3; i++) {
        ret += char_array_3[i];
      }
      i = 0;
    }
  }

  if (i) {
    for (j = i; j < 4; j++) {
      char_array_4[j] = 0;
    }
    for (j = 0; j < 4; j++) {
      char_array_4[j] = static_cast<unsigned char>(
          std::string(base64CharSet).find(char_array_4[j]));
    }

    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] =
        ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

    for (j = 0; (j < i - 1); j++) {
      ret += char_array_3[j];
    }
  }
  return ret;
}
