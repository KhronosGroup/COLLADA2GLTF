#include "COLLADA2GLTFWriter.h"
#include "COLLADASaxFWLLoader.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include <ctime>
#include <stdio.h>
#include <iostream>
#include <fstream>

int main(int argc, char **argv) {
	GLTF::Asset* asset = new GLTF::Asset();
	COLLADA2GLTF::Options* options = new COLLADA2GLTF::Options();

	for (int i = 1; i < argc; i++) {
		std::string arg = argv[i];
		if (arg == "-i" || arg == "--input") {
			i++;
			if (i < argc) {
				arg = argv[i];
				options->inputPath = arg;
				COLLADABU::URI uri(arg);
				options->basePath = uri.getPathDir();
			}
		}
		else if (arg == "-o" || arg == "--output") {
			i++;
			if (i < argc) {
				arg = argv[i];
				options->outputPath = arg;
			}
		}
		else if (arg == "--basePath") {
			i++;
			if (i < argc) {
				arg = argv[i];
				options->basePath = arg;
			}
		}
		else if (arg == "-s" || arg == "--separate") {
			options->embeddedBuffers = false;
			options->embeddedTextures = false;
		}
		else if (arg == "-t" || arg == "--separateImage") {
			options->embeddedTextures = false;
		}
		else if (arg == "-b" || arg == "--binary") {
			options->binary = true;
		}
		else {
			std::cout << "Error: Unknown flag or argument '" << arg << "'";
			return -1;
		}
	}

	std::clock_t start = std::clock();

	COLLADA2GLTF::Writer* writer = new COLLADA2GLTF::Writer(asset, options);
	COLLADASaxFWL::Loader* loader = new COLLADASaxFWL::Loader();
	COLLADAFW::Root root(loader, writer);
	if (!root.loadDocument(options->inputPath)) {
		std::cout << "Error: Unable to load input from path '" << options->inputPath << "'";
		return -1;
	}

	asset->separateSkeletonNodes();
	asset->removeUnusedNodes();
	asset->removeUnusedSemantics();
	GLTF::Buffer* buffer = asset->packAccessors();

	// Create image bufferViews for binary glTF
	if (options->binary) {
		size_t imageBufferLength = 0;
		std::vector<GLTF::Image*> images = asset->getAllImages();
		for (GLTF::Image* image : images) {
			imageBufferLength += image->byteLength;
		}
		unsigned char* bufferData = buffer->data;
		bufferData = (unsigned char*)realloc(bufferData, buffer->byteLength + imageBufferLength);
		size_t byteOffset = buffer->byteLength;
		for (GLTF::Image* image : images) {
			GLTF::BufferView* bufferView = new GLTF::BufferView(byteOffset, image->byteLength, buffer);
			image->bufferView = bufferView;
			std::memcpy(bufferData + byteOffset, image->data, image->byteLength);
			byteOffset += image->byteLength;
		}
		buffer->data = bufferData;
		buffer->byteLength += imageBufferLength;
		asset->extensionsUsed.insert("KHR_binary_glTF");
	}

	rapidjson::StringBuffer s;
	rapidjson::Writer<rapidjson::StringBuffer> jsonWriter = rapidjson::Writer<rapidjson::StringBuffer>(s);
	jsonWriter.StartObject();
	asset->writeJSON(&jsonWriter, options);
	jsonWriter.EndObject();

	if (!options->embeddedTextures) {
		COLLADABU::URI outputPath(options->outputPath);
		std::string outputDir = outputPath.getPathDir();
		for (GLTF::Image* image : asset->getAllImages()) {
			std::string uri = outputDir + image->uri;
			FILE* file = fopen(uri.c_str(), "wb");
			if (file != NULL) {
				fwrite(image->data, sizeof(unsigned char), image->byteLength, file);
				fclose(file);
			}
			else {
				std::cout << "Error: Couldn't write image to path '" << uri << "'";
			}
		}
	}

	if (!options->embeddedBuffers) {
		COLLADABU::URI outputPath(options->outputPath);
		std::string outputDir = outputPath.getPathDir();
		std::string uri = outputDir + buffer->uri;
		FILE* file = fopen(uri.c_str(), "wb");
		if (file != NULL) {
			fwrite(buffer->data, sizeof(unsigned char), buffer->byteLength, file);
			fclose(file);
		}
		else {
			std::cout << "Error: Couldn't write buffer to path '" << uri << "'";
		}
	}

	std::string jsonString = s.GetString();
	if (!options->binary) {
		std::ofstream file(options->outputPath);
		if (file.is_open()) {
			file << jsonString;
			file.close();
		}
		else {
			std::cout << "Error couldn't write glTF to path '" << options->outputPath << "'";
		}
	}
	else {
		std::string outputPath = options->outputPath;
		COLLADABU::URI outputUri(outputPath);
		if (outputUri.getPathExtension() != "glb") {
			outputPath = outputUri.getPathDir() + outputUri.getPathFileBase() + ".glb";
		}
		FILE* file = fopen(outputPath.c_str(), "wb");
		if (file != NULL) {
			uint32_t* writeHeader = new uint32_t[4];
			fwrite("glTF", sizeof(char), 4, file); // magic
			writeHeader[0] = 1; // version
			int padding = (20 + jsonString.length()) % 4;
			if (padding != 0) {
				padding = 4 - padding;
			}
			writeHeader[1] = 20 + jsonString.length() + padding + buffer->byteLength; // length
			writeHeader[2] = jsonString.length() + padding; // contentLength
			writeHeader[3] = 0; // contentFormat
			fwrite(writeHeader, sizeof(uint32_t), 4, file); 
			fwrite(jsonString.c_str(), sizeof(char), jsonString.length(), file);
			for (int i = 0; i < padding; i++) {
				fwrite(" ", sizeof(char), 1, file);
			}
			fwrite(buffer->data, sizeof(unsigned char), buffer->byteLength, file);
			fclose(file);
		}
		else {
			std::cout << "Error couldn't write binary glTF to path '" << outputPath << "'";
		}
	}

	std::clock_t end = std::clock();
	std::cout << "Conversion time: " << ((end - start) / (double)(CLOCKS_PER_SEC / 1000)) << " ms" << std::endl;
}
