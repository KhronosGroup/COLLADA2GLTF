#pragma once

#include <set>

#include "GeneratedSaxParser.h"
#include "COLLADASaxFWLIExtraDataCallbackHandler.h"

namespace COLLADA2GLTF {
	class ExtrasHandler : COLLADASaxFWL::IExtraDataCallbackHandler {
		virtual bool elementBegin(const COLLADASaxFWL::ParserChar* elementName, const GeneratedSaxParser::xmlChar** attributes);

		virtual bool elementEnd(const COLLADASaxFWL::ParserChar* elementName) { return true; }
		virtual bool textData(const COLLADASaxFWL::ParserChar* text, size_t textLength) { return true; }

		virtual bool parseElement(
			const COLLADASaxFWL::ParserChar* profileName,
			const COLLADASaxFWL::StringHash& elementHash,
			const COLLADAFW::UniqueId& uniqueId,
			COLLADAFW::Object* object);

		COLLADAFW::UniqueId _currentId;
	public:
		std::set<COLLADAFW::UniqueId> lockAmbientDiffuse;
	};
}