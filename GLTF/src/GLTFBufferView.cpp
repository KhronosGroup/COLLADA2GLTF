#include "GLTFBufferView.h"

GLTF::BufferView::BufferView(unsigned char* data,
  int dataLength,
  GLTF::Constants::WebGL target
) {
  this->byteOffset = 0;
  this->byteLength = dataLength;
  this->buffer = new Buffer(data, dataLength);
  this->target = target;
}
