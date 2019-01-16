# COLLADA2GLTF E2E Validation
This subproject implements COLLADA2GLTF end-to-end validation.

COLLADA source models are pulled from the
[glTF-Sample-Models](https://github.com/KhronosGroup/glTF-Sample-Models)
repository and the built converter binary is used to convert them to glTF.

The
[glTF-Validator](https://github.com/KhronosGroup/glTF-Validator)
is then used to validate the converted models, to make sure that they:
 * Have the expected number of primitives
 * Use the expected glTF features:
   * Animations
   * Morph Targets
   * Skins
   * Textures
 * Have no:
   * Errors
   * Infos
   * Warnings

These tests are implemented as a set of ES6 Mocha tests. You can run them
with:

```bash
npm install

npm run test
```
