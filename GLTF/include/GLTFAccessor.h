#pragma once

#include <string>

#include "GLTFBufferView.h"
#include "GLTFConstants.h"
#include "GLTFObject.h"

namespace GLTF {
  class Accessor : public GLTF::Object {
  public:
    enum class Type {
      SCALAR,
      VEC2,
      VEC3,
      VEC4,
      MAT2,
      MAT3,
      MAT4
    };

    GLTF::BufferView* bufferView;
    int byteOffset;
    int byteStride;
    GLTF::Constants::WebGL componentType;
    int count;
    double* max;
    double* min;
    Type type;

    Accessor(GLTF::Accessor::Type type,
      GLTF::Constants::WebGL componentType
    ) : type(type), componentType(componentType), byteOffset(0), byteStride(0) {};

    Accessor(GLTF::Accessor::Type type,
      GLTF::Constants::WebGL componentType,
      unsigned char* data,
      int dataLength,
      GLTF::Constants::WebGL target
    );

    bool computeMinMax();
    int getByteStride();
    bool getComponentAtIndex(int index, double* component);
    int getComponentByteLength();
    int getNumberOfComponents();
  };
};
