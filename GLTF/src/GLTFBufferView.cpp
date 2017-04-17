#include "GLTFBufferView.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

GLTF::BufferView::BufferView(int byteOffset, int byteLength, GLTF::Buffer* buffer) {
	this->byteOffset = byteOffset;
	this->byteLength = byteLength;
	this->buffer = buffer;
}

GLTF::BufferView::BufferView(unsigned char* data, int dataLength) {
	this->byteOffset = 0;
	this->byteLength = dataLength;
	this->buffer = new Buffer(data, dataLength);
}

GLTF::BufferView::BufferView(unsigned char* data, int dataLength, GLTF::Constants::WebGL target) : GLTF::BufferView::BufferView(data, dataLength) {
	this->target = target;
}

void GLTF::BufferView::writeJSON(void* writer, GLTF::Options* options) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;
	if (this->buffer) {
		jsonWriter->Key("buffer");
		jsonWriter->Int(this->buffer->id);
	}
	jsonWriter->Key("byteOffset");
	jsonWriter->Int(this->byteOffset);
	jsonWriter->Key("byteLength");
	jsonWriter->Int(this->byteLength);
	if ((int)target > 0) {
		jsonWriter->Key("target");
		jsonWriter->Int((int)this->target);
	}
}