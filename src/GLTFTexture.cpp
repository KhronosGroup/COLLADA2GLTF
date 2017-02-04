#include "GLTFTexture.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

namespace GLTF {
	void GLTF::Texture::writeJSON(void* writer) {
		rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;

		jsonWriter->Key("format");
		jsonWriter->Int((int)format);
		jsonWriter->Key("internalFormat");
		jsonWriter->Int((int)internalFormat);
		if (sampler) {
			jsonWriter->Key("sampler");
			jsonWriter->String(sampler->id.c_str());
		}
		if (source) {
			jsonWriter->Key("source");
			jsonWriter->String(isource->id.c_str());
		}
		jsonWriter->Key("target");
		jsonWriter->Int((int)target);
		jsonWriter->Key("type");
		jsonWriter->Int((int)type);
	}
}