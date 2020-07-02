// Copyright 2020 The Khronos® Group Inc.
#include "COLLADA2GLTFExtrasHandler.h"

#include <COLLADAFW.h>
#include <COLLADASaxFWLFileLoader.h>
#include <COLLADASaxFWLLibraryEffectsLoader.h>

bool COLLADA2GLTF::ExtrasHandler::elementBegin(
    const COLLADASaxFWL::ParserChar* elementName,
    const GeneratedSaxParser::xmlChar** attributes) {
  std::string name = std::string(elementName);
  if (name == "ambient_diffuse_lock") {
    lockAmbientDiffuse.insert(_currentId);
  } else if (name == "bump") {
    _inBump = true;
  } else if (_inBump && name == "texture" && attributes != NULL) {
    const COLLADASaxFWL::FileLoader* fileLoader = _loader->getFileLoader();
    COLLADASaxFWL::LibraryEffectsLoader* effectsLoader =
        (COLLADASaxFWL::LibraryEffectsLoader*)fileLoader->getPartLoader();
    COLLADAFW::Effect* bumpEffect =
        (COLLADAFW::Effect*)effectsLoader->getObject();
    COLLADAFW::TextureAttributes* bumpTexture =
        bumpEffect->createExtraTextureAttributes();
    bumpTextures[_currentId] = bumpTexture;

    size_t index = 0;
    const GeneratedSaxParser::xmlChar* attributeKey = attributes[index++];
    const GeneratedSaxParser::xmlChar* attributeValue = 0;
    while (attributeKey != 0) {
      std::string key = std::string(attributeKey);
      attributeValue = attributes[index++];
      if (attributeValue != 0) {
        std::string value = std::string(attributeValue);
        if (key == "texture") {
          bumpTexture->textureSampler = std::string(attributeValue);
        } else if (key == "texcoord") {
          bumpTexture->texCoord = std::string(attributeValue);
        }
      }
      attributeKey = attributes[index++];
    }
  } else if (name == "double_sided") {
    _inDoubleSided = true;
  }
  return true;
}

bool COLLADA2GLTF::ExtrasHandler::elementEnd(
    const COLLADASaxFWL::ParserChar* elementName) {
  std::string name = std::string(elementName);
  if (name == "bump") {
    _inBump = false;
  } else if (name == "double_sided") {
    _inDoubleSided = false;
  }
  return true;
}

bool COLLADA2GLTF::ExtrasHandler::parseElement(
    const COLLADASaxFWL::ParserChar* profileName,
    const COLLADASaxFWL::StringHash& elementHash,
    const COLLADAFW::UniqueId& uniqueId, COLLADAFW::Object* object) {
  _currentId = uniqueId;
  return true;
}

bool COLLADA2GLTF::ExtrasHandler::textData(
    const COLLADASaxFWL::ParserChar* text, size_t textLength) {
  if (_inDoubleSided) {
    std::string flag = std::string(text, textLength);
    if (flag == "1" || flag == "true") {
      doubleSided.insert(_currentId);
    }
  }
  return true;
}
