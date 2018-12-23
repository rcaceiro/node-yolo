# Changelog
**Note**: In this project the version semantic will be x.y.z, when:
- **z** represent, backwards compatible bug fixes are introduced
- **y** represent, new backwards compatible functionality are introduced
- **x** represent, any backwards incompatible changes are introduced, and/or a big bundle of bug fixes and new functionalities are introduced.


## [v2.1.0](https://github.com/rcaceiro/node-yolo/tree/v2.0.0) (23-12-2018)
[Full Changelog](https://github.com/rcaceiro/node-yolo/compare/v2.0.0...v2.1.0)

**New Features**:
- node-yolo now can process only x number of frames. 1/3 means process 1 frames for each 3, the default behaviour is process every frame.
- node-yolo now allow the developer can specify the threshold, the default behavior is 0.5.

**Implemented enhancements:**
- now on reject the node-yolo no longer kill nodeJS.

## [v2.0.1](https://github.com/rcaceiro/node-yolo/tree/v2.0.0) (23-12-2018)
[Full Changelog](https://github.com/rcaceiro/node-yolo/compare/v2.0.0...v2.0.1)

**Fixed bugs:**
- Small fix then yolo_status_decode is called, it wasn't linked on libyolo

**Closed issues:**
Detect only first object from same class [\#4](https://github.com/rcaceiro/node-yolo/issues/4)

## [v2.0.0](https://github.com/rcaceiro/node-yolo/tree/v2.0.0) (15-11-2018)
[Full Changelog](https://github.com/rcaceiro/node-yolo/compare/previous_to_v2.0.0...v2.0.0)

**New Features**:
- node-yolo classify videos, but for now only video files, not a camera stream.
- added to results of classification the time that took to be classified.

**Implemented enhancements:**
- Creation of a changelog
- Added a capability on Makefile to detect OpenMP, only on Linux, if macOs users has it installed has to change manually the parameter OPENMP in Makefile.
- Added constraints on CPU and OS to install this module in package.json

**Fixed bugs:**
- Fix on Makefile to include more new object files, needed

**Backwards Incompatibility:**
- The versions prior 2.0.0 of the module has the [ImageMagick](https://www.imagemagick.org) as a dependency, but with OpenCV we can archive the desired goal. And by this we remove one dependency of the project.

**Closed issues:**
- Getting Empty Array In Detections Instead Of Object [\#1](https://github.com/rcaceiro/node-yolo/issues/1)
- Built with GPU stuff enabled but using CPU [\#3](https://github.com/rcaceiro/node-yolo/issues/3)