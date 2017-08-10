#pragma once

//#ifdef USE_DRACO

#include <map>
#include <string>
#include <unordered_map>

#include "GLTFAccessor.h"
#include "GLTFBufferView.h"
#include "GLTFConstants.h"
#include "GLTFObject.h"

namespace GLTF {
 /* 
  class DracoAttribute : public GLTF::Object {
    public:
      DracoAttribute(const std::string& semantic,
          GLTF::Constants::WebGL componentType,
          GLTF::Accessor::Type type) :
        semantic(semantic), componentType(componentType), type(type) {}
      
      GLTF::Constants::WebGL componentType;
      std::string semantic;
      Accessor::Type type = Accessor::Type::UNKNOWN;
      
      const char* getTypeName();
      virtual void writeJSON(void* writer, GLTF::Options* options);
  };
  */
  
  class DracoExtension : public GLTF::Object {
    public:
     DracoExtension() {
       // TODO: Get it from encoder.
      version = "0.9.1";
     } 
      GLTF::BufferView* bufferView = NULL;
      std::string version;
      //std::vector<DracoAttribute*> attributes;
      std::unordered_map<std::string, int> attribute_to_id;
		
      virtual void writeJSON(void* writer, GLTF::Options* options);
  };
}

//#endif // USE_DRACO
