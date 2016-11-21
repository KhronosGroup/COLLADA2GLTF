# OpenCOLLADA Modules
The included `modules` directory contains trimmed cmake build targets for OpenCOLLADA modules, specifically `COLLADAFramework` and `COLLADABaseUtils`.

Currently the OpenCOLLADA cmake build process requires global macros declared in the top-level cmake file, which is why it has been circumvented here. If that is corrected, these shims can be removed, and OpenCOLLADA cmake targets should be depended on directly.
