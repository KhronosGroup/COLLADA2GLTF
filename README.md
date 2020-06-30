<p align="center">
  <img src="https://raw.githubusercontent.com/KhronosGroup/glTF/master/specification/figures/gltf.png"/>
</p>

# COLLADA2GLTF
[![Build Status](https://github.com/KhronosGroup/COLLADA2GLTF/workflows/CI/badge.svg)](https://github.com/KhronosGroup/COLLADA2GLTF/actions?query=workflow%3ACI)

# COLLADA to glTF converter

A command-line tool to convert COLLADA (`.dae`) files to [glTF](https://github.com/KhronosGroup/glTF).

## Releases

Compiled binaries for Windows, MacOS, and Linux can be found under [releases](https://github.com/KhronosGroup/COLLADA2GLTF/releases). It is recommended to use the last versioned release

## Compile from source

1. Clone repository

  ```bash
  git clone --recursive https://github.com/KhronosGroup/COLLADA2GLTF.git
  ```
2. Compile

  ```bash
  cd COLLADA2GLTF
  mkdir build
  cd build
  cmake .. #-Dtest=ON
  # Linux
  make
  # Windows
  ## Open the generated COLLADA2GLTF.sln in Visual Studio and build
  ```

3. Run

  ```bash
  COLLADA2GLTF-bin[.exe]
  ```

4. Run tests

  ```bash
  COLLADA2GLTF-test[.exe]
  GLTF-test[.exe]
  ```

## Usage

```bash
COLLADA2GLTF[.exe] [input] [output] [options]
```
### Options
| Flag | Default | Required | Description |
| --- | --- | --- | --- |
| -i, --input | | Yes :white_check_mark: | Path to the input COLLADA file |
| -o, --output | output/${input}.gltf | No | Path to the output glTF file |
| --basepath | Parent of input path | No | Resolve external uris using this as the reference path |
| -s, --separate | false | No | Output separate binary buffer, shaders, and textures |
| -t, --separateTextures | false | No | Output textures separately |
| -b, --binary | false | No | Output Binary glTF |
| -m, --materialsCommon | false | No | Output materials using the KHR_materials_common extension |
| -v, --version | | No | glTF version to output (e.g. '1.0', '2.0') |
| -d, --dracoCompression | false | No | Output meshes using Draco compression extension |
| --qp | | No | Quantization bits used for position attributes in Draco compression extension |
| --qn | | No | Quantization bits used for normal attributes in Draco compression extension |
| --qt | | No | Quantization bits used for texcoord attributes in Draco compression extension |
| --qc | | No | Quantization bits used for color attributes in Draco compression extension |
| --qj | | No | Quantization bits used for joint indice and weight attributes in Draco compression extension |
| --metallicRoughnessTextures | | No | Paths to images to use as the PBR metallicRoughness textures |
| --specularGlossiness | false | No | output PBR materials with the KHR_materials_pbrSpecularGlossiness extension |
| --lockOcclusionMetallicRoughness | false | No | Set `metallicRoughnessTexture` to be the same as the `occlusionTexture` in materials where an ambient texture is defined |
| --doubleSided | false | No | Force all materials to be double sided. When this value is true, back-face culling is disabled and double sided lighting is enabled |
| --preserveUnusedSemantics | false | No | Don't optimize out primitive semantics and their data, even if they aren't used. |
