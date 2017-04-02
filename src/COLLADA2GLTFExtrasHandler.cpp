#include "COLLADA2GLTFExtrasHandler.h"

bool COLLADA2GLTF::ExtrasHandler::elementBegin(const COLLADASaxFWL::ParserChar* elementName, const GeneratedSaxParser::xmlChar** attributes) {
	if (std::string(elementName) == "ambient_diffuse_lock") {
		lockAmbientDiffuse.insert(_currentId);
	}
	return true;
}

bool COLLADA2GLTF::ExtrasHandler::parseElement(
	const COLLADASaxFWL::ParserChar* profileName,
	const COLLADASaxFWL::StringHash& elementHash,
	const COLLADAFW::UniqueId& uniqueId,
	COLLADAFW::Object* object) {
	_currentId = uniqueId;
	return true;
}