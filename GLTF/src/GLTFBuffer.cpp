#include "GLTFBuffer.h"

GLTF::Buffer::Buffer(unsigned char* data, int dataLength) {
  this->data = data;
  this->byteLength = dataLength;
}