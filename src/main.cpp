// Copyright 2020 The Khronos® Group Inc.
#include <stdio.h>

#include <ctime>
#include <fstream>
#include <iostream>

#include "COLLADA2GLTFExtrasHandler.h"
#include "COLLADA2GLTFWriter.h"
#include "COLLADASaxFWLLoader.h"
#include "ahoy/ahoy.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

const int HEADER_LENGTH = 12;
const int CHUNK_HEADER_LENGTH = 8;

int main(int argc, const char** argv) {
  COLLADA2GLTF::Options* options = new COLLADA2GLTF::Options();

  bool separate;
  bool separateTextures;

  ahoy::Parser* parser = new ahoy::Parser();
  parser->name("COLLADA2GLTF")
      ->usage("./COLLADA2GLTF input.dae output.gltf [options]");

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
      ->description(
          "output materials with glsl shaders using the KHR_technique_webgl "
          "extension");

  parser->define("m", &options->materialsCommon)
      ->alias("materialsCommon")
      ->defaults(false)
      ->description(
          "output materials using the KHR_materials_common extension");

  parser->define("doubleSided", &options->doubleSided)
      ->defaults(false)
      ->description(
          "Force all materials to be double sided. When this value is true, "
          "back-face culling is disabled and double sided lighting is enabled");

  parser->define("v", &options->version)
      ->alias("version")
      ->description("glTF version to output (e.g. '1.0', '2.0')");

  parser->define("p", &options->preserveUnusedSemantics)
      ->alias("preserveUnusedSemantics")
      ->defaults(false)
      ->description(
          "should unused semantics be preserved. When this value is true, all "
          "mesh data is left intact even if it's not used.");

  parser
      ->define("metallicRoughnessTextures",
               &options->metallicRoughnessTexturePaths)
      ->description(
          "paths to images to use as the PBR metallicRoughness textures");

  parser->define("specularGlossiness", &options->specularGlossiness)
      ->defaults(false)
      ->description(
          "output PBR materials with the KHR_materials_pbrSpecularGlossiness "
          "extension");

  parser
      ->define("lockOcclusionMetallicRoughness",
               &options->lockOcclusionMetallicRoughness)
      ->defaults(false)
      ->description(
          "set metallicRoughnessTexture to be the same as the occlusionTexture "
          "in materials where an ambient texture is defined");

  parser->define("d", &options->dracoCompression)
      ->alias("dracoCompression")
      ->defaults(false)
      ->description(
          "compress the geometries using Draco compression extension");

  parser->define("qp", &options->positionQuantizationBits)
      ->description(
          "position quantization bits used in Draco compression extension");

  parser->define("qn", &options->normalQuantizationBits)
      ->description(
          "normal quantization bits used in Draco compression extension");

  parser->define("qt", &options->texcoordQuantizationBits)
      ->description(
          "texture coordinate quantization bits used in Draco compression "
          "extension");

  parser->define("qc", &options->colorQuantizationBits)
      ->description(
          "color quantization bits used in Draco compression extension");

  parser->define("qj", &options->jointQuantizationBits)
      ->description(
          "joint indices and weights quantization bits used in Draco "
          "compression extension");

  if (parser->parse(argc, argv)) {
    // Resolve and sanitize paths
    COLLADABU::URI inputPathURI =
        COLLADABU::URI::nativePathToUri(options->inputPath);
    std::string inputPathDir;
    std::string inputPathBaseName;
    std::string inputPathExtension;
    inputPathURI.pathComponents(inputPathDir, inputPathBaseName,
                                inputPathExtension);
    COLLADABU::URI inputPathDirURI =
        COLLADABU::URI::nativePathToUri(inputPathDir);
    inputPathDir =
        inputPathDirURI.toNativePath(COLLADABU::Utils::getSystemType());

    options->name = inputPathBaseName;

    COLLADABU::URI outputPathURI;
    if (options->outputPath == "") {
      outputPathURI = COLLADABU::URI::nativePathToUri(
          inputPathDir + "output/" + inputPathBaseName + ".gltf");
      options->outputPath =
          outputPathURI.toNativePath(COLLADABU::Utils::getSystemType());
    }
    outputPathURI = COLLADABU::URI::nativePathToUri(options->outputPath);
    std::string outputPathDir;
    std::string outputPathBaseName;
    std::string outputPathExtension;
    outputPathURI.pathComponents(outputPathDir, outputPathBaseName,
                                 outputPathExtension);
    COLLADABU::URI outputPathDirURI =
        COLLADABU::URI::nativePathToUri(outputPathDir);
    outputPathDir =
        outputPathDirURI.toNativePath(COLLADABU::Utils::getSystemType());

    if (options->binary && outputPathExtension != "glb") {
      outputPathURI = COLLADABU::URI::nativePathToUri(
          outputPathDir + outputPathBaseName + ".glb");
      options->outputPath =
          outputPathURI.toNativePath(COLLADABU::Utils::getSystemType());
    }

    if (options->basePath == "") {
      options->basePath = inputPathDir;
    } else {
      COLLADABU::URI basePathURI =
          COLLADABU::URI::nativePathToUri(options->basePath);
      options->basePath =
          basePathURI.toNativePath(COLLADABU::Utils::getSystemType());
    }

    // Export flags
    if (separate != 0) {
      options->embeddedBuffers = false;
      options->embeddedShaders = false;
      options->embeddedTextures = false;
    }
    if (separateTextures != 0) {
      options->embeddedTextures = false;
    }

    if (options->version == "1.0" && !options->materialsCommon) {
      options->glsl = true;
    }

    if (options->glsl && options->materialsCommon) {
      std::cout
          << "ERROR: Cannot export with both glsl and materialsCommon enabled"
          << std::endl;
      return -1;
    }
    if ((options->glsl || options->materialsCommon) &&
        options->specularGlossiness) {
      std::cout << "ERROR: Cannot enable specularGlossiness unless the "
                   "materials are exported as PBR"
                << std::endl;
      return -1;
    }
    if ((options->glsl || options->materialsCommon) &&
        options->lockOcclusionMetallicRoughness) {
      std::cout << "ERROR: Cannot enable lockOcclusionMetallicRoughness unless "
                   "the materials are exported as PBR"
                << std::endl;
      return -1;
    }

    // Create the output directory if it does not exist

    if (!COLLADABU::Utils::directoryExists(outputPathDir)) {
      COLLADABU::Utils::createDirectoryIfNeeded(outputPathDir);
    }

    std::cout << "Converting " << options->inputPath << " -> "
              << options->outputPath << std::endl;
    std::clock_t start = std::clock();

    GLTF::Asset* asset = new GLTF::Asset();
    COLLADASaxFWL::Loader* loader = new COLLADASaxFWL::Loader();
    COLLADA2GLTF::ExtrasHandler* extrasHandler =
        new COLLADA2GLTF::ExtrasHandler(loader);
    COLLADA2GLTF::Writer* writer =
        new COLLADA2GLTF::Writer(loader, asset, options, extrasHandler);
    loader->registerExtraDataCallbackHandler(
        (COLLADASaxFWL::IExtraDataCallbackHandler*)extrasHandler);
    COLLADAFW::Root root(loader, writer);
    if (!root.loadDocument(options->inputPath)) {
      std::cout << "ERROR: Unable to load input from path '"
                << options->inputPath << "'" << std::endl;
      return -1;
    }

    asset->mergeAnimations(writer->getAnimationGroups());
    asset->removeUnusedNodes(options);

    if (!options->preserveUnusedSemantics) {
      asset->removeUnusedSemantics();
    }

    if (options->dracoCompression) {
      asset->removeUncompressedBufferViews();
      asset->compressPrimitives(options);
    }

    GLTF::Buffer* buffer = asset->packAccessors();
    if (options->binary && options->version == "1.0") {
      buffer->stringId = "binary_glTF";
    }

    // Create image bufferViews for binary glTF
    if (options->binary && options->embeddedTextures) {
      size_t imageBufferLength = 0;
      std::vector<GLTF::Image*> images = asset->getAllImages();
      for (GLTF::Image* image : images) {
        imageBufferLength += image->byteLength;
      }
      unsigned char* bufferData = buffer->data;
      bufferData = (unsigned char*)realloc(
          bufferData, buffer->byteLength + imageBufferLength);
      size_t byteOffset = buffer->byteLength;
      for (GLTF::Image* image : images) {
        GLTF::BufferView* bufferView =
            new GLTF::BufferView(byteOffset, image->byteLength, buffer);
        image->bufferView = bufferView;
        std::memcpy(bufferData + byteOffset, image->data, image->byteLength);
        byteOffset += image->byteLength;
      }
      buffer->data = bufferData;
      buffer->byteLength += imageBufferLength;
    }

    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> jsonWriter =
        rapidjson::Writer<rapidjson::StringBuffer>(s);
    jsonWriter.StartObject();
    asset->writeJSON(&jsonWriter, options);
    jsonWriter.EndObject();

    if (!options->embeddedTextures) {
      for (GLTF::Image* image : asset->getAllImages()) {
        COLLADABU::URI imageURI =
            COLLADABU::URI::nativePathToUri(outputPathDir + image->uri);
        std::string imageString =
            imageURI.toNativePath(COLLADABU::Utils::getSystemType());
        FILE* file = fopen(imageString.c_str(), "wb");
        if (file != NULL) {
          fwrite(image->data, sizeof(unsigned char), image->byteLength, file);
          fclose(file);
        } else {
          std::cout << "ERROR: Couldn't write image to path '" << imageString
                    << "'" << std::endl;
        }
      }
    }

    if (!options->embeddedBuffers) {
      COLLADABU::URI bufferURI =
          COLLADABU::URI::nativePathToUri(outputPathDir + buffer->uri);
      std::string bufferString =
          bufferURI.toNativePath(COLLADABU::Utils::getSystemType());
      FILE* file = fopen(bufferString.c_str(), "wb");
      if (file != NULL) {
        fwrite(buffer->data, sizeof(unsigned char), buffer->byteLength, file);
        fclose(file);
      } else {
        std::cout << "ERROR: Couldn't write buffer to path '" << bufferString
                  << "'" << std::endl;
      }
    }

    if (!options->embeddedShaders) {
      for (GLTF::Shader* shader : asset->getAllShaders()) {
        COLLADABU::URI shaderURI =
            COLLADABU::URI::nativePathToUri(outputPathDir + shader->uri);
        std::string shaderString =
            shaderURI.toNativePath(COLLADABU::Utils::getSystemType());
        FILE* file = fopen(shaderString.c_str(), "wb");
        if (file != NULL) {
          fwrite(shader->source.c_str(), sizeof(unsigned char),
                 shader->source.length(), file);
          fclose(file);
        } else {
          std::cout << "ERROR: Couldn't write shader to path '" << shaderString
                    << "'" << std::endl;
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
      } else {
        std::cout << "ERROR: couldn't write glTF to path '"
                  << options->outputPath << "'" << std::endl;
      }
    } else {
      FILE* file = fopen(options->outputPath.c_str(), "wb");
      if (file != NULL) {
        fwrite("glTF", sizeof(char), 4, file);  // magic

        uint32_t* writeHeader = new uint32_t[2];
        // version
        if (options->version == "1.0") {
          writeHeader[0] = 1;
        } else {
          writeHeader[0] = 2;
        }

        int jsonPadding = (4 - (jsonString.length() & 3)) & 3;
        int binPadding = (4 - (buffer->byteLength & 3)) & 3;

        writeHeader[1] =
            HEADER_LENGTH +
            (CHUNK_HEADER_LENGTH + jsonString.length() + jsonPadding +
             buffer->byteLength + binPadding);  // length
        if (options->version != "1.0") {
          writeHeader[1] += CHUNK_HEADER_LENGTH;
        }
        fwrite(writeHeader, sizeof(uint32_t), 2, file);  // GLB header

        writeHeader[0] =
            jsonString.length() +
            jsonPadding;  // 2.0 - chunkLength / 1.0 - contentLength
        if (options->version == "1.0") {
          writeHeader[1] = 0;  // 1.0 - contentFormat
        } else {
          writeHeader[1] = 0x4E4F534A;  // 2.0 - chunkType JSON
        }
        fwrite(writeHeader, sizeof(uint32_t), 2, file);
        fwrite(jsonString.c_str(), sizeof(char), jsonString.length(), file);
        for (int i = 0; i < jsonPadding; i++) {
          fwrite(" ", sizeof(char), 1, file);
        }
        if (options->version != "1.0") {
          writeHeader[0] = buffer->byteLength + binPadding;  // chunkLength
          writeHeader[1] = 0x004E4942;                       // chunkType BIN
          fwrite(writeHeader, sizeof(uint32_t), 2, file);
        }
        fwrite(buffer->data, sizeof(unsigned char), buffer->byteLength, file);
        for (int i = 0; i < binPadding; i++) {
          fwrite("\0", sizeof(char), 1, file);
        }
        delete[] writeHeader;

        fclose(file);
      } else {
        std::cout << "ERROR couldn't write binary glTF to path '"
                  << options->outputPath << "'" << std::endl;
      }
    }

    std::clock_t end = std::clock();
    std::cout << "Time: "
              << ((end - start) / static_cast<double>(CLOCKS_PER_SEC / 1000))
              << " ms" << std::endl;
    delete asset;
    return 0;
  } else {
    return -1;
  }
}
