#include "COLLADA2GLTFWriter.h"
#include "COLLADASaxFWLLoader.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include <stdio.h>
#include <iostream>
#include <fstream>

int main(int argc, char **argv) {
	GLTF::Asset* asset = new GLTF::Asset();
	COLLADA2GLTF::Options* options = new COLLADA2GLTF::Options();
	options->inputPath = "../input/BoxTextured.dae";
	options->outputPath = "../output/BoxTextured.gltf";

	COLLADA2GLTF::Writer* writer = new COLLADA2GLTF::Writer(asset, options);

	COLLADASaxFWL::Loader* loader = new COLLADASaxFWL::Loader();
	COLLADAFW::Root root(loader, writer);
	if (!root.loadDocument(options->inputPath)) {
		// TODO: Throw an error here
	}
	rapidjson::StringBuffer s;
	rapidjson::Writer<rapidjson::StringBuffer> jsonWriter = rapidjson::Writer<rapidjson::StringBuffer>(s);
	jsonWriter.StartObject();
	asset->writeJSON(&jsonWriter);
	jsonWriter.EndObject();
	std::string jsonString = s.GetString();
	std::ofstream file(options->outputPath);
	if (file.is_open()) {
		file << jsonString;
		file.close();
	}
}
