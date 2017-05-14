#include <iostream>
#include <map>

#include "Base64.h"
#include "GLTFImage.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

std::map<std::string, GLTF::Image*> _imageCache;

GLTF::Image::Image(std::string uri) : uri(uri) {}

GLTF::Image::Image(std::string uri, unsigned char* data, size_t byteLength, std::string fileExtension) : uri(uri), data(data), byteLength(byteLength) {
	if (std::string((char*)data, 1, 8) == "PNG\r\n\x1a\n") {
		mimeType = "image/png";
	}
	else if (data[0] == 255 && data[1] == 216) {
		mimeType = "image/jpeg";
	}
	else {
		mimeType = "image/" + fileExtension;
	}
}

GLTF::Image* GLTF::Image::load(path imagePath) {
	std::string fileString = imagePath.string();
	std::map<std::string, GLTF::Image*>::iterator imageCacheIt = _imageCache.find(fileString);
	if (imageCacheIt != _imageCache.end()) {
		return imageCacheIt->second;
	}
	std::string fileExtension = imagePath.extension().string();
	GLTF::Image* image = NULL;
	FILE* file = fopen(fileString.c_str(), "rb");
	if (file == NULL) {
		std::cout << "WARNING: Image uri: " << fileString << " could not be resolved " << std::endl;
		image = new GLTF::Image(imagePath.filename().string());
	}
	else {
		fseek(file, 0, SEEK_END);
		long int size = ftell(file);
		fclose(file);
		file = fopen(fileString.c_str(), "rb");
		unsigned char* buffer = (unsigned char*)malloc(size);
		int bytesRead = fread(buffer, sizeof(unsigned char), size, file);
		fclose(file);
		image = new GLTF::Image(imagePath.filename().string(), buffer, bytesRead, fileExtension);
	}
	_imageCache[fileString] = image;
	return image;
}

uint16_t endianSwap16(uint16_t x){
	return (x >> 8) | (x << 8);
}

uint32_t endianSwap32(uint32_t x) {
	return ((x >> 24) & 0xFF) | ((x << 8) & 0xFF0000) | ((x >> 8) & 0xFF00) | ((x << 24) & 0xFF000000);
}

/**
 * Get the dimensions of this image based on the mimeType.
 * Based on code from: https://github.com/image-size/image-size.
 */
std::pair<int, int> GLTF::Image::getDimensions() {
	int width = -1;
	int height = -1;
	if (mimeType == "image/png") {
		uint32_t* readUInt32 = (uint32_t*)(data + 16);
		height = endianSwap32(readUInt32[0]);
		width = endianSwap32(readUInt32[1]);
	}
	else if (mimeType == "image/jpeg") {
		// Skip signature chars
		size_t offset = 4;
		
		uint16_t i;
		unsigned char next;
		while (offset < byteLength) {
			uint16_t* readUint16 = (uint16_t*)(data + offset);
			i = endianSwap16(readUint16[0]);
			next = (data + offset)[i + 1];

			// 0xFFC0 is baseline(SOF)
			// 0xFFC2 is progressive(SOF2)
			if (next == 0xC0 || next == 0xC2) {
				readUint16 = (uint16_t*)(data + offset + i + 5);
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

void GLTF::Image::writeJSON(void* writer, GLTF::Options* options) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;

	if (options->embeddedTextures && data != NULL) {
		if (!options->binary) {
			jsonWriter->Key("uri");
			std::string embeddedUri = "data:" + mimeType + ";base64," + Base64::encode(data, byteLength);
			jsonWriter->String(embeddedUri.c_str());
		}
		else {
			jsonWriter->Key("bufferView");
			jsonWriter->Int(bufferView->id);
			jsonWriter->Key("mimeType");
			jsonWriter->String(mimeType.c_str());
		}
	}
	else {
		jsonWriter->Key("uri");
		jsonWriter->String(uri.c_str());
	}
	GLTF::Object::writeJSON(writer, options);
}