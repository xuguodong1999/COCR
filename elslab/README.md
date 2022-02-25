# clion config

```shell
# ubuntu host build for linux platform
-G "Ninja" -DBUILD_TESTS=ON -DBUILD_ELS_LAB=ON -DBUILD_TUTORIALS=ON -DOpenCV_DIR=~/shared/opencv4/lib/cmake/opencv4 -Dncnn_DIR=~/shared/ncnn/lib/cmake/ncnn -DBoost_DIR=~/shared/boost/lib/cmake/Boost-1.77.0 -DTorch_DIR=~/shared/libtorch/share/cmake/Torch -DQT_DIR=~/shared/Qt/5.15.2/gcc_64/lib/cmake/Qt5
# ubuntu host build for android platform
-G "Ninja" -DQt5_DIR=~/shared/Qt/5.15.2/android/lib/cmake/Qt5 -DQT_DIR=~/shared/Qt/5.15.2/android/lib/cmake/Qt5 -DCMAKE_TOOLCHAIN_FILE=~/Android/Sdk/ndk/21.4.7075529/build/cmake/android.toolchain.cmake -DANDROID_PLATFORM=30 -DANDROID_ABI=arm64-v8a -DJAVA_HOME=~/shared/jdk1.8.0_30

cmake ../dev -G "Ninja" -DQt5_DIR=~/shared/Qt/5.15.2/android/lib/cmake/Qt5 -DQT_DIR=~/shared/Qt/5.15.2/android/lib/cmake/Qt5 -DCMAKE_TOOLCHAIN_FILE=~/Android/Sdk/ndk/21.4.7075529/build/cmake/android.toolchain.cmake -DANDROID_PLATFORM=30 -DANDROID_ABI=arm64-v8a -DJAVA_HOME=~/shared/jdk1.8.0_30 -DANDROID_NATIVE_API_LEVEL=24 -DANDROID_BUILD_ABI_arm64-v8a=ON -DCMAKE_BUILD_TYPE=Release

cmake --build . -j $(nproc) --config Release

~/shared/Qt/5.15.2/android/bin/androiddeployqt --input ~/source/repos/build-dev-Android_Qt_5_15_2_android_Clang_Multi_Abi-Release/android_deployment_settings.json --output ~/source/repos/build-dev-Android_Qt_5_15_2_android_Clang_Multi_Abi-Release/android-build --android-platform android-30 --jdk ~/shared/jdk1.8.0_301 --gradle 
```

# cmake build

```shell
cmake --build . -j $(nproc) --config Release --target install
```

# build opencv

```shell
cmake -G "Ninja" ~/git/opencv -DCMAKE_INSTALL_PREFIX=~/shared/opencv4 -DOPENCV_EXTRA_MODULES_PATH=~/git/opencv_contrib/modules -DVTK_DIR=~/shared/vtk/lib/cmake/vtk-9.0 -DWITH_CUDA=ON -DOPENCV_DNN_CUDA=ON -DWITH_QT=ON -DWITH_OPENGL=ON -DWITH_VTK=ON -DBUILD_opencv_js=OFF -DBUILD_opencv_js_bindings_generator=OFF -DBUILD_JAVA=OFF -DBUILD_opencv_java_bindings_generator=OFF -DBUILD_opencv_python_bindings_generator=OFF -DBUILD_opencv_python_tests=OFF -DBUILD_TESTS=OFF -DBUILD_PERF_TESTS=OFF -DBUILD_SHARED_LIBS=ON -DINSTALL_PDB_COMPONENT_EXCLUDE_FROM_ALL=OFF -DBUILD_DOCS=OFF -DCMAKE_BUILD_TYPE=Release
```

# build libtorch

```shell
sudo apt-get install python3-typing-extensions python3-pretty-yaml libmkldnn-dev liblapack-dev
git clone --recursive https://github.com/pytorch/pytorch
git submodule sync
git submodule update --init --recursive

cmake -G "Ninja" ~/git/pytorch -DCMAKE_INSTALL_PREFIX=~/shared/libtorch -DOpenCV_DIR=~/shared/opencv4/lib/cmake/opencv4 -DBUILD_PYTHON=OFF -DUSE_OPENCV=ON -DUSE_DISTRIBUTED=OFF -DUSE_TENSORPIPE=OFF -DUSE_CUDA=ON -DUSE_MKLDNN=ON -DUSE_NCCL=OFF -DUSE_NUMPY=OFF -DCMAKE_BUILD_TYPE=Release -DBUILD_DOCS=ON -DCMAKE_INSTALL_DOCDIR=~/shared/libtorch/doc
```
