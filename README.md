# COCR

[![License](https://img.shields.io/badge/License-GPLv3-blue.svg)](./LICENSE.md)

**COCR** is designed to convert an image of **hand-written** chemical structure to **graph** of that molecule.

COCR, **O**ptical **C**haracter **R**ecognition for **C**hemical Structures, was once a demo for my undergraduate
graduation thesis in 2021.6. It brings OCSR(optical chemical structure recognition) capability into handwriting cases. Below is a summary of supported items.

|symbol|looks like|supported|
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

In this repository, you can find stable version from [release pages](https://github.com/xuguodong1999/COCR/tags). Master branch is under development as it is not robust enough.

## Features
### v1.1
* Add support on strings and wavy bonds
### v1.0
* Support single element symbols: C、H、O、N、P、B、S、F、Cl、Br、I.
* Support bond types: single, double, triple, hash wedge, solid wedge, circle.

<table width="100%" border="0" cellspacing="0" cellpadding="0">
<tr>
  <td align="center"><img src="assets/img/origin.png" width="auto" height="auto"/></td>
  <td align="center"><img src="assets/img/soso17.png" width="auto" height="auto"/></td>
  <td align="center"><img src="assets/img/stick-and-ball.png" width="auto" height="auto"/></td>
</tr>
<tr>
  <td align="center">Input</td>
  <td align="center">Detection</td>
  <td align="center">Render</td>
  </tr>
</table>

## Build from source
OpenCV ≥4.5.1, Qt =5.15.2, ncnn ≥20210322 but ≤20210720 are required for a minimal build.
```shell
git clone https://github.com/xuguodong1999/COCR.git
cd COCR && mkdir build && cd build

cmake .. -G "Ninja" \
-DBUILD_TESTS=OFF \
-DQt5_DIR=path/to/Qt/5.15.2/gcc_64/lib/cmake/Qt5 \
-DOpenCV_DIR=path/to/opencv4/lib/cmake/opencv4 \
-Dncnn_DIR=path/to/ncnn/lib/cmake/ncnn

cmake --build . -j $(nprocs) --config Release
```
For 3rdparty unit tests, add -DBUILD_TESTS=ON option. Boost is required, for example,
```shell
-DBUILD_TESTS=ON \
-DBoost_DIR=path/to/boost/lib/cmake/Boost-1.77.0
```
Under data/models, YOLO and CRNN weights are both available in ncnn format. Additionally, YOLO in darknet and CRNN in onnx are provided.

## Data generation
COCR uses [SCUT-COUCH2009](https://www.hcii-lab.net/data/scutcouch/CN/couch.html) as meta handwriting data, and uses QtGui::QTextDocument as rich text renderer. 

A chemical structure generator for handwriting cases is written to provider training data for YOLO and CRNN models, which composes meta-character into random chemical structure formulas. You can find related codes under src/data_gen.

After a minimal build above, a data_gen(.exe) can be found under $(BUILD_DIR)/out. There are following usages:
* Double click or run from shell WITHOUT arguments

* Run with -yolo [number of samples] [an empty, existing directory path], for example,

```shell
# generate 10 object detection samples under ./yolo/
./data_gen -yolo 10 ./
```

* Run with -crnn [number of samples] [an empty, existing directory path], for example,

```shell
# generate 10 text recognition samples under ./crnn/
./data_gen -crnn 10 ./
```

* Run with -isomer [number of samples] [an empty, existing directory path], for example,

```shell
# generate all alkane isomers for C-num ≤ 10 under ./isomer/
# dont play with numver over 20 without taking a look at src/data_gen/isomers.cpp.
# it may comsume a lot of memory and cpus.
./data_gen -isomer 16 ./
```

## Desktop and mobile application

### Platforms ( tested )
* Linux
* Windows
* Android
* macos

### Cheminformat Support
* OpenBabel was made to work well with Qt's rcc system, and has been used as forcefield or format provider in COCR.

### Stick-and-ball render
* Use Qt3D C++ API.

### DNN backend
* Use either OpenCV or ncnn.

## Acknowledgements
* Development of COCR project was once supported by NANJING UNIVERSITY SOFTWARE INSTITUTE as a National College Student Innovation and Entrepreneurship Training Program.

## License
[GPLv3 Clause](./LICENSE.md)
