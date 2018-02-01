#include "COLLADA2GLTFWriter.h"
#include "COLLADA2GLTFExtrasHandler.h"
#include "COLLADASaxFWLLoader.h"

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include "ahoy/ahoy.h"

#include <ctime>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <experimental/filesystem>

using namespace ahoy;
using namespace std::experimental::filesystem;

const int HEADER_LENGTH = 12;
const int CHUNK_HEADER_LENGTH = 8;

int main(int argc, const char **argv) {
	GLTF::Asset* asset = new GLTF::Asset();
	COLLADA2GLTF::Options* options = new COLLADA2GLTF::Options();

	bool separate;
	bool separateTextures;

	Parser* parser = new Parser();
	parser->name("COLLADA2GLTF")->usage("./COLLADA2GLTF input.dae output.gltf [options]");

	parser->define("i", &options->inputPath)
		->alias("input")
		->description("path of the input COLLADA file")
		->index(0)
		->require();

	parser->define("o", &options->outputPath)
		->alias("output")
		->description("path of the output glTF file")
		->index(1);

	parser->define("basePath", &options->basePath)
		->description("resolve external uris using this as the reference path");

	parser->define("s", &separate)
		->alias("separate")
		->defaults(false)
		->description("output separate binary buffer, shaders, and textures");

	parser->define("t", &separateTextures)
		->alias("separateTextures")
		->defaults(false)
		->description("output images separately, but embed buffers and shaders");

	parser->define("b", &options->binary)
		->alias("binary")
		->defaults(false)
		->description("output binary glTF");

	parser->define("g", &options->glsl)
		->alias("glsl")
		->defaults(false)
		->description("output materials with glsl shaders using the KHR_technique_webgl extension");

	parser->define("m", &options->materialsCommon)
		->alias("materialsCommon")
		->defaults(false)
		->description("output materials using the KHR_materials_common extension");

	parser->define("metallicRoughnessTextures", &options->metallicRoughnessTexturePaths)
		->description("paths to images to use as the PBR metallicRoughness textures");

	parser->define("specularGlossiness", &options->specularGlossiness)
		->defaults(false)
		->description("output PBR materials with the KHR_materials_pbrSpecularGlossiness extension");

	parser->define("lockOcclusionMetallicRoughness", &options->lockOcclusionMetallicRoughness)
		->defaults(false)
		->description("set metallicRoughnessTexture to be the same as the occlusionTexture in materials where an ambient texture is defined");

	parser->define("d", &options->dracoCompression)
		->alias("dracoCompression")
		->defaults(false)
		->description("compress the geometries using Draco compression extension");

	parser->define("qp", &options->positionQuantizationBits)
		->description("position quantization bits used in Draco compression extension");

	parser->define("qn", &options->normalQuantizationBits)
		->description("normal quantization bits used in Draco compression extension");

	parser->define("qt", &options->texcoordQuantizationBits)
		->description("texture coordinate quantization bits used in Draco compression extension");

	parser->define("qc", &options->colorQuantizationBits)
		->description("color quantization bits used in Draco compression extension");

	parser->define("qj", &options->jointQuantizationBits)
		->description("joint indices and weights quantization bits used in Draco compression extension");

	parser->define("uint", &options->useUintIndices)
		->defaults(false)
		->description("use OES_element_index_uint extension to allow 32-bit vertex indices");

	if (parser->parse(argc, argv)) {
		// Resolve and sanitize paths
		path inputPath = path(options->inputPath);
		options->inputPath = inputPath.string();
		options->name = inputPath.stem().string();

		path outputPath;
		if (options->outputPath == "") {
			outputPath = inputPath.parent_path() / "output" / inputPath.stem();
			outputPath += ".gltf";
		}
		else {
			outputPath = path(options->outputPath);
		}
		if (options->binary && outputPath.extension() != "glb") {
			outputPath = outputPath.parent_path() / outputPath.stem();
			outputPath += ".glb";
		}
		options->outputPath = outputPath.string();

		path basePath;
		if (options->basePath == "") {
			basePath = inputPath.parent_path();
		}
		else {
			basePath = path(options->basePath);
		}
		options->basePath = basePath.string();

		// Export flags
		if (separate != 0) {
			options->embeddedBuffers = false;
			options->embeddedShaders = false;
			options->embeddedTextures = false;
		}
		if (separateTextures != 0) {
			options->embeddedTextures = false;
		}

		if (options->glsl && options->materialsCommon) {
			std::cout << "ERROR: Cannot export with both glsl and materialsCommon enabled" << std::endl;
			return -1;
		}
		if ((options->glsl || options->materialsCommon) && options->specularGlossiness) {
			std::cout << "ERROR: Cannot enable specularGlossiness unless the materials are exported as PBR" << std::endl;
			return -1;
		}
		if ((options->glsl || options->materialsCommon) && options->lockOcclusionMetallicRoughness) {
			std::cout << "ERROR: Cannot enable lockOcclusionMetallicRoughness unless the materials are exported as PBR" << std::endl;
			return -1;
		}

		// Create the output directory if it does not exist
		path outputDirectory = outputPath.parent_path();
		if (!std::experimental::filesystem::exists(outputDirectory)) {
			std::experimental::filesystem::create_directories(outputDirectory);
		}

		std::cout << "Converting " << options->inputPath << " -> " << options->outputPath << std::endl;
		std::clock_t start = std::clock();

		COLLADASaxFWL::Loader* loader = new COLLADASaxFWL::Loader();
		COLLADA2GLTF::ExtrasHandler* extrasHandler = new COLLADA2GLTF::ExtrasHandler(loader);
		COLLADA2GLTF::Writer* writer = new COLLADA2GLTF::Writer(asset, options, extrasHandler);
		loader->registerExtraDataCallbackHandler((COLLADASaxFWL::IExtraDataCallbackHandler*)extrasHandler);
		COLLADAFW::Root root(loader, writer);
		if (!root.loadDocument(options->inputPath)) {
			std::cout << "ERROR: Unable to load input from path '" << options->inputPath << "'" << std::endl;
			return -1;
		}

		asset->mergeAnimations();
		asset->removeUnusedNodes(options);
		asset->removeUnusedSemantics();

		if (options->dracoCompression) {
			asset->removeUncompressedBufferViews();
			asset->compressPrimitives(options);
		}

		GLTF::Buffer* buffer = asset->packAccessors();

		// Create image bufferViews for binary glTF
		if (options->binary && options->embeddedTextures) {
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
		}

		rapidjson::StringBuffer s;
		rapidjson::Writer<rapidjson::StringBuffer> jsonWriter = rapidjson::Writer<rapidjson::StringBuffer>(s);
		jsonWriter.StartObject();
		asset->writeJSON(&jsonWriter, options);
		jsonWriter.EndObject();

		if (!options->embeddedTextures) {
			for (GLTF::Image* image : asset->getAllImages()) {
				path uri = outputDirectory / image->uri;
				FILE* file = fopen(uri.generic_string().c_str(), "wb");
				if (file != NULL) {
					fwrite(image->data, sizeof(unsigned char), image->byteLength, file);
					fclose(file);
				}
				else {
					std::cout << "ERROR: Couldn't write image to path '" << uri << "'" << std::endl;
				}
			}
		}

		if (!options->embeddedBuffers) {
			path uri = outputDirectory / buffer->uri;
			FILE* file = fopen(uri.generic_string().c_str(), "wb");
			if (file != NULL) {
				fwrite(buffer->data, sizeof(unsigned char), buffer->byteLength, file);
				fclose(file);
			}
			else {
				std::cout << "ERROR: Couldn't write buffer to path '" << uri << "'" << std::endl;
			}
		}

		if (!options->embeddedShaders) {
			for (GLTF::Shader* shader : asset->getAllShaders()) {
				path uri = outputDirectory / shader->uri;
				FILE* file = fopen(uri.generic_string().c_str(), "wb");
				if (file != NULL) {
					fwrite(shader->source.c_str(), sizeof(unsigned char), shader->source.length(), file);
					fclose(file);
				}
				else {
					std::cout << "ERROR: Couldn't write shader to path '" << uri << "'" << std::endl;
				}
			}
		}

		std::string jsonString = s.GetString();
		if (!options->binary) {
			rapidjson::Document jsonDocument;
			jsonDocument.Parse(jsonString.c_str());

			rapidjson::StringBuffer buffer;
			rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
			jsonDocument.Accept(writer);

			std::ofstream file(options->outputPath);
			if (file.is_open()) {
				file << buffer.GetString() << std::endl;
				file.close();
			}
			else {
				std::cout << "ERROR: couldn't write glTF to path '" << options->outputPath << "'" << std::endl;
			}
		}
		else {
			FILE* file = fopen(outputPath.generic_string().c_str(), "wb");
			if (file != NULL) {
				fwrite("glTF", sizeof(char), 4, file); // magic

				uint32_t* writeHeader = new uint32_t[2];
				writeHeader[0] = 2; // version

				int jsonPadding = (4 - (jsonString.length() & 3)) & 3;
				int binPadding = (4 - (buffer->byteLength & 3)) & 3;

				writeHeader[1] = HEADER_LENGTH +
					(CHUNK_HEADER_LENGTH + jsonString.length() + jsonPadding) +
					(CHUNK_HEADER_LENGTH + buffer->byteLength + binPadding); // length
				fwrite(writeHeader, sizeof(uint32_t), 2, file); // GLB header

				writeHeader[0] = jsonString.length() + jsonPadding; // chunkLength
				writeHeader[1] = 0x4E4F534A; // chunkType JSON
				fwrite(writeHeader, sizeof(uint32_t), 2, file);
				fwrite(jsonString.c_str(), sizeof(char), jsonString.length(), file);
				for (int i = 0; i < jsonPadding; i++) {
					fwrite(" ", sizeof(char), 1, file);
				}

				writeHeader[0] = buffer->byteLength + binPadding; // chunkLength
				writeHeader[1] = 0x004E4942; // chunkType BIN
				fwrite(writeHeader, sizeof(uint32_t), 2, file);
				fwrite(buffer->data, sizeof(unsigned char), buffer->byteLength, file);
				for (int i = 0; i < binPadding; i++) {
					fwrite("\0", sizeof(char), 1, file);
				}

				fclose(file);
			}
			else {
				std::cout << "ERROR couldn't write binary glTF to path '" << outputPath << "'" << std::endl;
			}
		}

		std::clock_t end = std::clock();
		std::cout << "Time: " << ((end - start) / (double)(CLOCKS_PER_SEC / 1000)) << " ms" << std::endl;
		return 0;
	}
	else {
		return -1;
	}
}
