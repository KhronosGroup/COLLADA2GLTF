#include <algorithm>
#include <limits>

#include <stdio.h>

#include "GLTFAccessor.h"

GLTF::Accessor::Accessor(GLTF::Accessor::Type type,
  GLTF::Constants::WebGL componentType,
  unsigned char* data,
  int dataLength,
  GLTF::Constants::WebGL target
) : Accessor(type, componentType) {
  this->bufferView = new GLTF::BufferView(data, dataLength, target);
  this->count = dataLength / this->getComponentByteLength() / this->getNumberOfComponents();
  this->computeMinMax();
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
    double component[numberOfComponents];
    this->getComponentAtIndex(0, component);
    for (int i = 0; i < numberOfComponents; i++) {
      min[i] = component[i];
      max[i] = component[i];
    }
    for (int i = 1; i < this->count; i++) {
      this->getComponentAtIndex(i, component);
      for (int j = 0; j < numberOfComponents; j++) {
        max[j] = std::max(component[j], max[j]);
      }
    }
  }
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

int GLTF::Accessor::getComponentByteLength() {
  switch (this->componentType) {
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

int GLTF::Accessor::getNumberOfComponents() {
  switch (this->type) {
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
