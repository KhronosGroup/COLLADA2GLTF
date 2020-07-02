// Copyright 2020 The Khronos® Group Inc.
#pragma once

#include <map>
#include <set>
#include <vector>

#include "COLLADASaxFWLIExtraDataCallbackHandler.h"
#include "COLLADASaxFWLLoader.h"
#include "GeneratedSaxParser.h"

namespace COLLADA2GLTF {
class ExtrasHandler : COLLADASaxFWL::IExtraDataCallbackHandler {
 private:
  virtual bool elementBegin(const COLLADASaxFWL::ParserChar* elementName,
                            const GeneratedSaxParser::xmlChar** attributes);
  virtual bool elementEnd(const COLLADASaxFWL::ParserChar* elementName);
  virtual bool textData(const COLLADASaxFWL::ParserChar* text,
                        size_t textLength);

  virtual bool parseElement(const COLLADASaxFWL::ParserChar* profileName,
                            const COLLADASaxFWL::StringHash& elementHash,
                            const COLLADAFW::UniqueId& uniqueId,
                            COLLADAFW::Object* object);

  COLLADASaxFWL::Loader* _loader;
  COLLADAFW::UniqueId _currentId;
  bool _inBump = false;
  bool _inDoubleSided = false;

 public:
  std::set<COLLADAFW::UniqueId> lockAmbientDiffuse;
  std::map<COLLADAFW::UniqueId, COLLADAFW::TextureAttributes*> bumpTextures;
  std::set<COLLADAFW::UniqueId> doubleSided;
  explicit ExtrasHandler(COLLADASaxFWL::Loader* loader) : _loader(loader) {}
};
}  // namespace COLLADA2GLTF
