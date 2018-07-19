Change Log
==========
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
