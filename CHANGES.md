Change Log
==========
### v2.1.5 - 2019-05-22

##### Additions :tada:
* Added support for morph targets [#166](https://github.com/KhronosGroup/COLLADA2GLTF/issues/166)
* Support converting models with transparency [#168](https://github.com/KhronosGroup/COLLADA2GLTF/issues/168)
* Added support for exporting COLLADA animation clips as groups [#227](https://github.com/KhronosGroup/COLLADA2GLTF/pull/227)
* Clean up glTF tree when the asset is freed [#229](https://github.com/KhronosGroup/COLLADA2GLTF/issues/229)
* Added `--preserveUnusedSemantics` option [#245](https://github.com/KhronosGroup/COLLADA2GLTF/pull/245)
* Added end-to-end validation to CI [#235](https://github.com/KhronosGroup/COLLADA2GLTF/pull/235)

##### Fixes :wrench:
* Library nodes should only be written as part of a scene [#236](https://github.com/KhronosGroup/COLLADA2GLTF/issues/236)

### v2.1.4 - 2018-08-29

##### Additions :tada:
* Added support for multiple maps [#169](https://github.com/KhronosGroup/COLLADA2GLTF/issues/169)

##### Fixes :wrench:
* Fixed issue with relative path resolution on Windows [#200](https://github.com/KhronosGroup/COLLADA2GLTF/issues/200)
* Updated to OpenCOLLADA 1.6.63
  * Resolves issue where image elements declared in profile_COMMON are not written [#129](https://github.com/KhronosGroup/COLLADA2GLTF/issues/129) and [#114](https://github.com/KhronosGroup/COLLADA2GLTF/issues/114)
* Remove Windows debug builds from CI - as per [Microsoft](https://docs.microsoft.com/en-us/previous-versions/visualstudio/visual-studio-6.0/aa260978(v=vs.60)#a-list-of-redistributable-files), they are not to be redistributed

### v2.1.3 - 2018-07-01

##### Additions :tada:
* Enable OS X builds in CI [#191](https://github.com/KhronosGroup/COLLADA2GLTF/pull/191)

##### Fixes :wrench:
* Bone weights are normalized, resolving some rendering issues using logarithmic depth buffers [#187](https://github.com/KhronosGroup/COLLADA2GLTF/pull/187)

### v2.1.2 - 2018-04-22

##### Fixes :wrench:
* Upgrade to Draco 1.2.5 [#179](https://github.com/KhronosGroup/COLLADA2GLTF/pull/179)
* Fix issue with exporting normal maps [#182](https://github.com/KhronosGroup/COLLADA2GLTF/pull/182)
* Fix issue with COLLADA textures with no backing image [#183](https://github.com/KhronosGroup/COLLADA2GLTF/pull/183)

### v2.1.1 - 2018-04-04

##### Additions :tada:
* Add support for exporting double sided materials [#133](https://github.com/KhronosGroup/COLLADA2GLTF/pull/133)

##### Fixes :wrench:
* Fixed issue where incorrect mime types were being generated [#162](https://github.com/KhronosGroup/COLLADA2GLTF/pull/164)

### v2.1.0 - 2018-03-15

##### Additions :tada:
* Add support for exporting glTF 1.0 [#69](https://github.com/KhronosGroup/COLLADA2GLTF/issues/69)

##### Fixes :wrench:
* Export UNSIGNED_INT indices for large primitives [#123](https://github.com/KhronosGroup/COLLADA2GLTF/issues/123)
* Fixed issues with mesh instancing [#135](https://github.com/KhronosGroup/COLLADA2GLTF/issues/135)

### v2.0.0 - 2017-08-27

##### Highlights :sparkler:
* Rewrite of legacy converter for exporting glTF 2.0 [#3](https://github.com/KhronosGroup/COLLADA2GLTF/issues/3)
