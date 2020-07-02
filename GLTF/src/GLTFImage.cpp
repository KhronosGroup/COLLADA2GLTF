// Copyright 2020 The Khronos® Group Inc.
#include "GLTFImage.h"

#include <algorithm>
#include <iostream>
#include <map>

#include "Base64.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

std::map<std::string, GLTF::Image*> _imageCache;

GLTF::Image::Image(std::string uri, std::string cacheKey)
    : uri(uri), cacheKey(cacheKey) {}

GLTF::Image::Image(std::string uri) : Image(uri, "") {}

GLTF::Image::Image(std::string uri, std::string cacheKey, unsigned char* data,
                   size_t byteLength, std::string fileExtension)
    : uri(uri), data(data), byteLength(byteLength), cacheKey(cacheKey) {
  std::string dataSubstring(reinterpret_cast<char*>(data), 8);
  if (dataSubstring.substr(1, 7) == "PNG\r\n\x1a\n") {
    mimeType = "image/png";
  } else if (data[0] == 255 && data[1] == 216) {
    mimeType = "image/jpeg";
  } else {
    mimeType = "image/" + fileExtension;
  }
}

GLTF::Image::Image(std::string uri, unsigned char* data, size_t byteLength,
                   std::string fileExtension)
    : Image(uri, "", data, byteLength, fileExtension) {}

GLTF::Image::~Image() {
  if (!cacheKey.empty()) {
    _imageCache.erase(cacheKey);
  }

  free(this->data);
}

GLTF::Image* GLTF::Image::load(std::string imagePath, bool writeAbsoluteUris) {
  std::map<std::string, GLTF::Image*>::iterator imageCacheIt =
      _imageCache.find(imagePath);
  if (imageCacheIt != _imageCache.end()) {
    return imageCacheIt->second;
  }

  GLTF::Image* image = NULL;
  if (writeAbsoluteUris) {
    std::string absoluteUri = "file://";
    if (imagePath[0] != '/') {
      absoluteUri += '/';
    }
    absoluteUri += imagePath;

    std::replace(absoluteUri.begin(), absoluteUri.end(), '\\', '/');
    image = new GLTF::Image(absoluteUri, imagePath);
    _imageCache[imagePath] = image;
    return image;
  }

  size_t fileExtensionStart = imagePath.find_last_of(".");
  std::string fileExtension = "";
  if (fileExtensionStart != std::string::npos) {
    fileExtension = imagePath.substr(fileExtensionStart + 1);
  }

  size_t lastSlash = imagePath.find_last_of("/");
  std::string fileName = imagePath;
  if (lastSlash == std::string::npos) {
    lastSlash = imagePath.find_last_of("\\");
  }
  if (lastSlash != std::string::npos) {
    fileName = imagePath.substr(lastSlash + 1);
  }

  FILE* file = fopen(imagePath.c_str(), "rb");
  if (file == NULL) {
    std::cout << "WARNING: Image uri: " << imagePath
              << " could not be resolved " << std::endl;
    image = new GLTF::Image(fileName, imagePath);
  } else {
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fclose(file);
    file = fopen(imagePath.c_str(), "rb");
    unsigned char* buffer = (unsigned char*)malloc(size);
    size_t bytesRead = fread(buffer, sizeof(unsigned char), size, file);
    fclose(file);
    image =
        new GLTF::Image(fileName, imagePath, buffer, bytesRead, fileExtension);
  }
  _imageCache[imagePath] = image;
  return image;
}

uint16_t endianSwap16(uint16_t x) { return (x >> 8) | (x << 8); }

uint32_t endianSwap32(uint32_t x) {
  return ((x >> 24) & 0xFF) | ((x << 8) & 0xFF0000) | ((x >> 8) & 0xFF00) |
         ((x << 24) & 0xFF000000);
}

/**
 * Get the dimensions of this image based on the mimeType.
 * Based on code from: https://github.com/image-size/image-size.
 */
std::pair<int, int> GLTF::Image::getDimensions() {
  int width = -1;
  int height = -1;
  if (mimeType == "image/png") {
    uint32_t* readUInt32 = reinterpret_cast<uint32_t*>(data + 16);
    height = endianSwap32(readUInt32[0]);
    width = endianSwap32(readUInt32[1]);
  } else if (mimeType == "image/jpeg") {
    // Skip signature chars
    size_t offset = 4;

    uint16_t i;
    unsigned char next;
    while (offset < byteLength) {
      uint16_t* readUint16 = reinterpret_cast<uint16_t*>(data + offset);
      i = endianSwap16(readUint16[0]);
      next = (data + offset)[i + 1];

      // 0xFFC0 is baseline(SOF)
      // 0xFFC2 is progressive(SOF2)
      if (next == 0xC0 || next == 0xC2) {
        readUint16 = reinterpret_cast<uint16_t*>(data + offset + i + 5);
        height = endianSwap16(readUint16[0]);
        width = endianSwap16(readUint16[1]);
        break;
      }

      // next block
      offset += i + 2;
    }
  }
  return std::pair<int, int>(width, height);
}

std::string GLTF::Image::typeName() { return "image"; }

void GLTF::Image::writeJSON(void* writer, GLTF::Options* options) {
  rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter =
      (rapidjson::Writer<rapidjson::StringBuffer>*)writer;

  if (options->embeddedTextures && data != NULL) {
    if (!options->binary) {
      jsonWriter->Key("uri");
      std::string embeddedUri =
          "data:" + mimeType + ";base64," + Base64::encode(data, byteLength);
      jsonWriter->String(embeddedUri.c_str());
    } else {
      if (options->version == "1.0") {
        jsonWriter->Key("extensions");
        jsonWriter->StartObject();
        jsonWriter->Key("KHR_binary_glTF");
        jsonWriter->StartObject();
        jsonWriter->Key("bufferView");
        jsonWriter->String(bufferView->getStringId().c_str());
        jsonWriter->Key("mimeType");
        jsonWriter->String(mimeType.c_str());
        jsonWriter->Key("width");
        jsonWriter->Int(getDimensions().first);
        jsonWriter->Key("height");
        jsonWriter->Int(getDimensions().second);
        jsonWriter->EndObject();
        jsonWriter->EndObject();
      } else {
        jsonWriter->Key("bufferView");
        jsonWriter->Int(bufferView->id);
        jsonWriter->Key("mimeType");
        jsonWriter->String(mimeType.c_str());
      }
    }
  } else {
    jsonWriter->Key("uri");
    jsonWriter->String(uri.c_str());
  }
  GLTF::Object::writeJSON(writer, options);
}
