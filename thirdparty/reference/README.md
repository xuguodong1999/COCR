# A collection of code references for 3rdparty libraries

## CMake layout

```shell
# tree ./tutorials -L 1
tutorials
├── CMakeLists.txt
├── cpprl
│     ├── CMakeLists.txt
│     ├── dirs...
│     └── codes...@cpp
├── libtorch
│     ├── CMakeLists.txt
│     ├── proj_1
│     ├── proj_2
│     ├── proj...
│     └── proj_n
├── torch...@cpp
├── ob...@cpp
├── qt...@cpp
└── other_prefix...@cpp
```

## Link rules

code under ./, self-contained cpp files, will link to 3rdparty libraries according to file prefix.

1. torch...@cpp link to libtorch and opencv
2. ob...@cpp link to openbabel, coordgenlibs and maeparser
3. qt...@cpp link to qt

code under ./cpprl, link to libtorch, is a copy of

1. https://github.com/Omegastick/pytorch-cpp-rl.git

code under ./libtorch, link to libtorch and opencv, is a copy of

1. https://github.com/AllentDan/LibtorchSegmentation.git
2. https://github.com/AllentDan/LibtorchTutorials.git
3. https://github.com/prabhuomkar/pytorch-cpp.git
4. https://github.com/koba-jon/pytorch_cpp.git
5. https://github.com/pytorch/examples.git
