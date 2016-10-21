# COLLADA to glTF converter

A command-line tool to convert COLLADA (`.dae`) files to [glTF](https://github.com/KhronosGroup/glTF).

## Usage

```
collada2gltlf -f [file] [options]
options:
-z -> path of configuration file [string]
-f -> path of input file, argument [string]
-o -> path of output file argument [string]
-b -> path of output bundle argument [string]
-g -> [experimental] GLSL version to output in generated shaders
-i -> invert-transparency
-d -> export pass details to be able to regenerate shaders and states
-p -> output progress
-l -> enable default lighting (if no lights in scene) [bool], default:true
-c -> compression type: available: Open3DGC [string]
-m -> compression mode: for Open3DGC can be "ascii"(default) or "binary" [string]
-v -> print version
-s -> experimental mode
-h -> help
-r -> verbose logging
-e -> embed resources (bin, shaders, available textures) in glTF file
-n -> don't combine animations with the same target
-k -> export materials and lights using KHR_materials_common extension
```
