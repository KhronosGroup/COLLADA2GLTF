#pragma once

#include <string>

#include "GLTFBufferView.h"
#include "GLTFConstants.h"
#include "GLTFObject.h"

namespace GLTF {
  class Accessor : public GLTF::Object {
  public:
    static int INSTANCE_COUNT;
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
	  double* max = NULL;
	  double* min = NULL;
    Type type;

    Accessor(GLTF::Accessor::Type type, GLTF::Constants::WebGL componentType);

    Accessor(GLTF::Accessor::Type type,
      GLTF::Constants::WebGL componentType,
      unsigned char* data,
      int dataLength,
      GLTF::Constants::WebGL target
    );

    Accessor(GLTF::Accessor::Type type,
      GLTF::Constants::WebGL componentType,
      unsigned char* data,
      int dataLength,
      GLTF::BufferView* bufferView
    );

    bool computeMinMax();
    int getByteStride();
    bool getComponentAtIndex(int index, double* component);
    int getComponentByteLength();
    int getNumberOfComponents();
  };
};
