#include <algorithm>
#include <limits>
#include <cstring>
#include <stdlib.h>

#include "GLTFAccessor.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

GLTF::Accessor::Accessor(GLTF::Accessor::Type type,
	GLTF::Constants::WebGL componentType
) : type(type), componentType(componentType), byteOffset(0), byteStride(0) {}

GLTF::Accessor::Accessor(GLTF::Accessor::Type type,
	GLTF::Constants::WebGL componentType,
	unsigned char* data,
	int count,
	GLTF::Constants::WebGL target
) : Accessor(type, componentType) {
	int byteLength = count * this->getNumberOfComponents() * this->getComponentByteLength();
	unsigned char* allocatedData = (unsigned char*)malloc(byteLength);
	std::memcpy(allocatedData, data, byteLength);
	this->bufferView = new GLTF::BufferView(allocatedData, byteLength, target);
	this->count = count;
	this->computeMinMax();
}

GLTF::Accessor::Accessor(GLTF::Accessor::Type type,
	GLTF::Constants::WebGL componentType,
	unsigned char* data,
	int count,
	GLTF::BufferView* bufferView
) : Accessor(type, componentType) {
	GLTF::Buffer* buffer = bufferView->buffer;
	this->bufferView = bufferView;
	this->byteOffset = bufferView->byteLength;
	this->count = count;
	int componentByteLength = this->getComponentByteLength();
	int byteLength = count * this->getNumberOfComponents() * componentByteLength;

	int padding = byteOffset % componentByteLength;
	if (padding != 0) {
		padding = componentByteLength - padding;
	}
	this->byteOffset += padding;

	buffer->data = (unsigned char*)realloc(buffer->data, buffer->byteLength + padding + byteLength);
	std::memcpy(buffer->data + buffer->byteLength + padding, data, byteLength);
	buffer->byteLength += byteLength + padding;
	bufferView->byteLength += byteLength + padding;
	this->computeMinMax();
}

GLTF::Accessor::Accessor(GLTF::Accessor::Type type,
	GLTF::Constants::WebGL componentType,
	int byteOffset,
	int byteStride,
	int count,
	GLTF::BufferView* bufferView
) : Accessor(type, componentType) {
	this->byteOffset = byteOffset;
	this->byteStride = byteStride;
	this->count = count;
	this->bufferView = bufferView;
}

bool GLTF::Accessor::computeMinMax() {
	int numberOfComponents = this->getNumberOfComponents();
	int count = this->count;
	if (count > 0) {
		if (max == NULL) {
			max = new double[numberOfComponents];
		}
		if (min == NULL) {
			min = new double[numberOfComponents];
		}
		double* component = new double[numberOfComponents];
		this->getComponentAtIndex(0, component);
		for (int i = 0; i < numberOfComponents; i++) {
			min[i] = component[i];
			max[i] = component[i];
		}
		for (int i = 1; i < this->count; i++) {
			this->getComponentAtIndex(i, component);
			for (int j = 0; j < numberOfComponents; j++) {
				min[j] = std::min(component[j], min[j]);
				max[j] = std::max(component[j], max[j]);
			}
		}
	}
	return true;
}

int GLTF::Accessor::getByteStride() {
	if (this->byteStride == 0) {
		return this->getNumberOfComponents() * this->getComponentByteLength();
	}
	return this->byteStride;
}

bool GLTF::Accessor::getComponentAtIndex(int index, double* component) {
	int byteOffset = this->byteOffset + this->bufferView->byteOffset;
	int numberOfComponents = this->getNumberOfComponents();
	byteOffset += this->getByteStride() * index;
	unsigned char* buf = this->bufferView->buffer->data + byteOffset;

	for (int i = 0; i < numberOfComponents; i++) {
		switch (this->componentType) {
		case GLTF::Constants::WebGL::BYTE:
			component[i] = (double)((char*)buf)[i];
			break;
		case GLTF::Constants::WebGL::UNSIGNED_BYTE:
			component[i] = (double)buf[i];
			break;
		case GLTF::Constants::WebGL::SHORT:
			component[i] = (double)((short*)buf)[i];
			break;
		case GLTF::Constants::WebGL::UNSIGNED_SHORT:
			component[i] = (double)((unsigned short*)buf)[i];
			break;
		case GLTF::Constants::WebGL::FLOAT:
			component[i] = (double)((float*)buf)[i];
			break;
		case GLTF::Constants::WebGL::UNSIGNED_INT:
			component[i] = (double)((unsigned int*)buf)[i];
			break;
		default:
			return false;
		}
	}
	return true;
}

bool GLTF::Accessor::writeComponentAtIndex(int index, double* component) {
	int byteOffset = this->byteOffset + this->bufferView->byteOffset;
	int numberOfComponents = this->getNumberOfComponents();
	byteOffset += this->getByteStride() * index;
	unsigned char* buf = this->bufferView->buffer->data + byteOffset;

	for (int i = 0; i < numberOfComponents; i++) {
		switch (this->componentType) {
		case GLTF::Constants::WebGL::BYTE:
			buf[i] = (char)component[i];
			break;
		case GLTF::Constants::WebGL::UNSIGNED_BYTE:
			buf[i] = (unsigned char)component[i];
			break;
		case GLTF::Constants::WebGL::SHORT:
			((short*)buf)[i] = (short)component[i];
			break;
		case GLTF::Constants::WebGL::UNSIGNED_SHORT: 
			((unsigned short*)buf)[i] = (unsigned short)component[i];
			break;
		case GLTF::Constants::WebGL::FLOAT: 
			((float*)buf)[i] = (float)component[i];
			break;
		case GLTF::Constants::WebGL::UNSIGNED_INT:
			((unsigned int*)buf)[i] = (unsigned int)component[i];
			break;
		default:
			return false;
		}
	}
	return true;
}

int GLTF::Accessor::getComponentByteLength(GLTF::Constants::WebGL componentType) {
	switch (componentType) {
	case GLTF::Constants::WebGL::BYTE:
	case GLTF::Constants::WebGL::UNSIGNED_BYTE:
		return 1;
	case GLTF::Constants::WebGL::SHORT:
	case GLTF::Constants::WebGL::UNSIGNED_SHORT:
		return 2;
	case GLTF::Constants::WebGL::FLOAT:
	case GLTF::Constants::WebGL::UNSIGNED_INT:
		return 4;
	}
	return 0;
}

int GLTF::Accessor::getComponentByteLength() {
	return GLTF::Accessor::getComponentByteLength(this->componentType);
}

int GLTF::Accessor::getNumberOfComponents(GLTF::Accessor::Type type) {
	switch (type) {
	case GLTF::Accessor::Type::SCALAR:
		return 1;
	case GLTF::Accessor::Type::VEC2:
		return 2;
	case GLTF::Accessor::Type::VEC3:
		return 3;
	case GLTF::Accessor::Type::VEC4:
	case GLTF::Accessor::Type::MAT2:
		return 4;
	case GLTF::Accessor::Type::MAT3:
		return 9;
	case GLTF::Accessor::Type::MAT4:
		return 16;
	}
	return 0;
}

int GLTF::Accessor::getNumberOfComponents() {
	return GLTF::Accessor::getNumberOfComponents(this->type);
}

const char* GLTF::Accessor::getTypeName() {
	switch (this->type) {
	case GLTF::Accessor::Type::SCALAR:
		return "SCALAR";
	case GLTF::Accessor::Type::VEC2:
		return "VEC2";
	case GLTF::Accessor::Type::VEC3:
		return "VEC3";
	case GLTF::Accessor::Type::VEC4:
		return "VEC4";
	case GLTF::Accessor::Type::MAT2:
		return "MAT2";
	case GLTF::Accessor::Type::MAT3:
		return "MAT3";
	case GLTF::Accessor::Type::MAT4:
		return "MAT4";
	}
	return "";
}

bool GLTF::Accessor::equals(GLTF::Accessor* accessor) {
	if (type != accessor->type || componentType != accessor->componentType || count != accessor->count) {
		return false;
	}
	int numberOfComponents = getNumberOfComponents();
	double* componentOne = new double[numberOfComponents];
	double* componentTwo = new double[numberOfComponents];
	for (int i = 0; i < count; i++) {
		this->getComponentAtIndex(i, componentOne);
		accessor->getComponentAtIndex(i, componentTwo);
		for (int j = 0; j < numberOfComponents; j++) {
			if (componentOne[j] != componentTwo[j]) {
				return false;
			}
		}
	}
	return true;
}

void GLTF::Accessor::writeJSON(void* writer, GLTF::Options* options) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;
	if (this->bufferView) {
		jsonWriter->Key("bufferView");
		jsonWriter->Int(this->bufferView->id);
	}
	jsonWriter->Key("byteOffset");
	jsonWriter->Int(this->byteOffset);
	jsonWriter->Key("byteStride");
	jsonWriter->Int(this->byteStride);
	jsonWriter->Key("componentType");
	jsonWriter->Int((int)this->componentType);
	jsonWriter->Key("count");
	jsonWriter->Int(this->count);
	if (this->max) {
		jsonWriter->Key("max");
		jsonWriter->StartArray();
		for (int i = 0; i < this->getNumberOfComponents(); i++) {
			jsonWriter->Double(this->max[i]);
		}
		jsonWriter->EndArray();
	}
	if (this->min) {
		jsonWriter->Key("min");
		jsonWriter->StartArray();
		for (int i = 0; i < this->getNumberOfComponents(); i++) {
			jsonWriter->Double(this->min[i]);
		}
		jsonWriter->EndArray();
	}
	jsonWriter->Key("type");
	jsonWriter->String(this->getTypeName());
}
