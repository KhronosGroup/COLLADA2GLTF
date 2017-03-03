#include "Base64.h"
#include "GLTFImage.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

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
			std::pair<int, int> dimensions = getDimensions();
			jsonWriter->Key("width");
			jsonWriter->Int(dimensions.first);
			jsonWriter->Key("height");
			jsonWriter->Int(dimensions.second);
		}
	}
	else {
		jsonWriter->Key("uri");
		jsonWriter->String(uri.c_str());
	}
	GLTF::Object::writeJSON(writer, options);
}