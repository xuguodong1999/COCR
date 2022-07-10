# clion config

## ubuntu host build for linux platform

```shell
cmake .. -G "Ninja" \
    -DBUILD_TESTS=ON \
    -DBUILD_PRIVATE=ON \
    -DOpenCV_DIR:PATH=~/shared/opencv4/lib/cmake/opencv4 \
    -DBoost_DIR:PATH=~/shared/boost/lib/cmake/Boost-1.77.0 \
    -DTorch_DIR:PATH=~/shared/libtorch/share/cmake/Torch \
    -DTorchVision_DIR:PATH=~/shared/libtorch/share/cmake/TorchVision \
    -DQt5_DIR:PATH=~/shared/Qt/5.15.2/gcc_64/lib/cmake/Qt5 \
    -DCMAKE_CUDA_COMPILER=/usr/local/cuda/bin/nvcc
```

* Config it in CLion Setting -> Build -> CMake -> cmake options

  `-G "Ninja" -DBUILD_TESTS=ON -DBUILD_PRIVATE=ON -DOpenCV_DIR:PATH=~/shared/opencv4/lib/cmake/opencv4 -DBoost_DIR:PATH=~/shared/boost/lib/cmake/Boost-1.77.0 -DTorch_DIR:PATH=~/shared/libtorch/share/cmake/Torch -DTorchVision_DIR:PATH=~/shared/libtorch/share/cmake/TorchVision -DQt5_DIR:PATH=~/shared/Qt/5.15.2/gcc_64/lib/cmake/Qt5 -DCMAKE_CUDA_COMPILER=/usr/local/cuda/bin/nvcc`

## windows host build for windows platform

```shell
cmake .. -G "Ninja" ^
    -DBUILD_TESTS=ON ^
    -DBUILD_PRIVATE=ON ^
    -DOpenCV_DIR:PATH=C:/Library/Shared/opencv4/x64/vc16/lib ^
    -DBoost_DIR:PATH=C:/Library/Shared/boost/lib/cmake/Boost-1.77.0 ^
    -DTorch_DIR:PATH=C:/Library/Shared/libtorch/share/cmake/Torch ^
    -DQt5_DIR:PATH=C:/Qt/5.15.2/msvc2019_64/lib/cmake/Qt5
```

* Config it in CLion Setting -> Build -> CMake -> cmake options

  `-G "Ninja" -DBUILD_TESTS=ON -DBUILD_PRIVATE=ON -DOpenCV_DIR:PATH=C:/Library/Shared/opencv4/x64/vc16/lib -DBoost_DIR:PATH=C:/Library/Shared/boost/lib/cmake/Boost-1.77.0 -DTorch_DIR:PATH=C:/Library/Shared/libtorch/share/cmake/Torch -DQt5_DIR:PATH=C:/Qt/5.15.2/msvc2019_64/lib/cmake/Qt5`

## cmake build

`cmake --build . -j $(nproc) --config Release --target leafxy`

`cmake --build . --parallel --config Release --target leafxy`

## macos qtcreator build for m1

`-DBUILD_TESTS=ON -DBUILD_PRIVATE=ON -DQt5_DIR:PATH=/opt/homebrew/Cellar/qt@5/5.15.2_2/lib/cmake/Qt5`

`export PATH=/opt/homebrew/bin:$PATH`
