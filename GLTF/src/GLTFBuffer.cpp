#include "GLTFBuffer.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

int GLTF::Buffer::INSTANCE_COUNT = 0;

GLTF::Buffer::Buffer(unsigned char* data, int dataLength) {
	this->id = "buffer_" + std::to_string(GLTF::Buffer::INSTANCE_COUNT);
	GLTF::Buffer::INSTANCE_COUNT++;
	this->data = data;
	this->byteLength = dataLength;
}

const std::string base64CharSet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
char* base64Encode(unsigned char* data, int length) {
	int base64Length = (std::ceil(length / 3.0) * 4) + 1;
	char* base64 = new char[base64Length];
	int index;
	int j = 0;
	for (int i = 0; i < length; i += 3) {
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
			}
			else {
				base64[j] = base64CharSet[index];
				base64[j + 1] = '=';
			}
		}
		else {
			base64[j] = base64CharSet[index];
			base64[j + 1] = '=';
			base64[j + 2] = '=';
		}
	}
	base64[base64Length - 1] = '\0';
	return base64;
}

void GLTF::Buffer::writeJSON(void* writer) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;
	jsonWriter->Key("byteLength");
	jsonWriter->Int(this->byteLength);
	// TODO: GLTF::Options for separate/embedded
	jsonWriter->Key("uri");
	jsonWriter->String(("data:application/octet-stream;base64," + std::string(base64Encode(this->data, this->byteLength))).c_str());
	jsonWriter->Key("type");
	jsonWriter->String("arraybuffer");
	GLTF::Object::writeJSON(writer);
}
