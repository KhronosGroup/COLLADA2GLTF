#include "GLTFBufferView.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

int GLTF::BufferView::INSTANCE_COUNT = 0;

GLTF::BufferView::BufferView(unsigned char* data, int dataLength) {
	this->id = "bufferView_" + std::to_string(GLTF::BufferView::INSTANCE_COUNT);
	GLTF::BufferView::INSTANCE_COUNT++;
	this->byteOffset = 0;
	this->byteLength = dataLength;
	this->buffer = new Buffer(data, dataLength);
}

GLTF::BufferView::BufferView(unsigned char* data, int dataLength, GLTF::Constants::WebGL target) : GLTF::BufferView::BufferView(data, dataLength) {
	this->target = target;
}

void GLTF::BufferView::writeJSON(void* writer) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;
	if (this->buffer) {
		jsonWriter->Key("buffer");
		jsonWriter->String(this->buffer->id.c_str());
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