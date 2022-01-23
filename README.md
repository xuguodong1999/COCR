# COCR

[![License](https://img.shields.io/badge/License-GPLv3-blue.svg)](./LICENSE.md)

**COCR** is designed to convert an image of **hand-written** chemical structure to **graph** of that molecule.

COCR, **O**ptical **C**haracter **R**ecognition for **C**hemical Structures, was once a demo for my undergraduate
graduation thesis in 2021.6 called leafxy. It brings OCSR(optical chemical structure recognition) ability into handwriting cases. Below is a summary of supported items.

|symbol|example|supported|
|---|---|---|
|strings|**(CH<sub>2</sub>)<sub>2</sub>COOEt**|✔️|
|rings|**⏣**|✔️|
|solid wedge|**▲**|✔️|
|hash wedge|**△**|✔️|
|wavy bond|**~**|✔️|
|single bond|**/**|✔️|
|double bond|**//**|✔️|
|triple bond|**///**|✔️|

COCR is developed under Qt framework. It handles images with YOLO and CRNN models using opencv or ncnn backend.

In this repository, you can find stable version from [release pages](https://github.com/xuguodong1999/COCR/tags). Master branch is under development.

## Release notes
### v1.1 leafxy
* Add support on strings and wavy bonds
### v1.0 COCR
* Support single element symbols: C、H、O、N、P、B、S、F、Cl、Br、I.
* Support bond types: single, double, triple, hash wedge, solid wedge, circle.
## Build from source
opencv >= 4.5.1, qt == 5.15.2, 20210322 <= ncnn <= 20210720 are required for a minimal build.
```shell
git clone https://github.com/xuguodong1999/COCR.git
cd COCR
mkdir build && cd build
cmake .. -G "Ninja" \
-DBUILD_TESTS=OFF \
-DQt5_DIR=path/to/Qt/5.15.2/gcc_64/lib/cmake/Qt5 \
-DOpenCV_DIR=path/to/opencv4/lib/cmake/opencv4 \
-Dncnn_DIR=path/to/ncnn/lib/cmake/ncnn
```
for -DBUILD_TESTS=ON option, Boost is required, for example,
```shell
-DBUILD_TESTS=ON \
-DBoost_DIR=path/to/boost/lib/cmake/Boost-1.77.0
```
Under data/models, YOLO and CRNN weights are both available in ncnn format. Additionally, YOLO in darknet and CRNN in onnx are provided.

## Generation of fake training data
TODO-

## History
TODO-

## License
TODO-
