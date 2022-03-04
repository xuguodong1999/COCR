# clion config

## ubuntu host build for linux platform

```shell
-G "Ninja" -DBUILD_TESTS=ON -DBUILD_ELS_LAB=ON -DBUILD_TUTORIALS=ON -DOpenCV_DIR:PATH=~/shared/opencv4/lib/cmake/opencv4 -Dncnn_DIR:PATH=~/shared/ncnn/lib/cmake/ncnn -DBoost_DIR:PATH=~/shared/boost/lib/cmake/Boost-1.77.0 -DTorch_DIR:PATH=~/shared/libtorch/share/cmake/Torch -DQt5_DIR:PATH=~/shared/Qt/5.15.2/gcc_64/lib/cmake/Qt5
```

## windows host build for windows platform

```shell
-G "Ninja" -DBUILD_TESTS=ON -DBUILD_ELS_LAB=ON -DBUILD_TUTORIALS=ON -DOpenCV_DIR:PATH=C:/Library/Shared/opencv4/x64/vc16/lib -Dncnn_DIR:PATH=C:/Library/Shared/ncnn/lib/cmake/ncnn -DBoost_DIR:PATH=C:/Library/Shared/boost/lib/cmake/Boost-1.77.0 -DTorch_DIR:PATH=C:/Library/Shared/libtorch/share/cmake/Torch -DQt5_DIR:PATH=C:/Qt/5.15.2/msvc2019_64/lib/cmake/Qt5
```

## cmake build

```shell
cmake --build . -j $(nproc) --config Release --target install
```