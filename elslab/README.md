# clion config

## ubuntu host build for linux platform

```shell
-G "Ninja" -DBUILD_TESTS=ON -DBUILD_ELS_LAB=ON -DBUILD_TUTORIALS=ON -DOpenCV_DIR:PATH=~/shared/opencv4/lib/cmake/opencv4 -Dncnn_DIR:PATH=~/shared/ncnn/lib/cmake/ncnn -DBoost_DIR:PATH=~/shared/boost/lib/cmake/Boost-1.77.0 -DTorch_DIR:PATH=~/shared/libtorch/share/cmake/Torch -DQt5_DIR:PATH=~/shared/Qt/5.15.2/gcc_64/lib/cmake/Qt5
```

## windows host build for windows platform

```shell
-G "Ninja" -DBUILD_TESTS=ON -DBUILD_ELS_LAB=ON -DBUILD_TUTORIALS=ON -DOpenCV_DIR:PATH=C:/Library/Shared/opencv4/x64/vc16/lib -Dncnn_DIR:PATH=C:/Library/Shared/ncnn/lib/cmake/ncnn -DBoost_DIR:PATH=C:/Library/Shared/boost/lib/cmake/Boost-1.77.0 -DTorch_DIR:PATH=C:/Library/Shared/libtorch/share/cmake/Torch -DQt5_DIR:PATH=C:/Qt/5.15.2/gcc_64/lib/cmake/Qt5
```

# android build

## ubuntu host build for android platform

```shell
# due to qt-community-lts issues, 
# qt-5.15.2 only support single project-level apk build
cmake .. -G "Ninja" -DQt5_DIR:PATH=~/shared/Qt/5.15.2/android/lib/cmake/Qt5 -DCMAKE_TOOLCHAIN_FILE=~/Android/Sdk/ndk/21.4.7075529/build/cmake/android.toolchain.cmake -DANDROID_PLATFORM=30 -DANDROID=ON -DANDROID_ABI=armeabi-v7a -DJAVA_HOME=~/shared/jdk1.8.0_30 -DANDROID_NATIVE_API_LEVEL=24 -DANDROID_BUILD_ABI_armeabi-v7a=ON -DANDROID_DEPLOY_QT:FILEPATH=~/shared/Qt/5.15.2/android/bin/androiddeployqt -DANDROID_SDK:PATH=~/Android/Sdk -DANDROID_NDK:PATH=~/Android/Sdk/ndk/21.4.7075529

~/shared/Qt/5.15.2/android/bin/androiddeployqt --input ./android_deployment_settings.json --output ./android-build --android-platform android-30 --jdk ~/shared/jdk1.8.0_301 --gradle

adb install  ./android-build/build/outputs/apk/debug/android-build-debug.apk
```

## windows host build for android platform

```bat
cmake .. -G "Ninja" -DQt5_DIR:PATH=C:/Qt/5.15.2/android/lib/cmake/Qt5 -DCMAKE_TOOLCHAIN_FILE=C:/Users/xgd/AppData/Local/Android/Sdk/ndk/21.4.7075529/build/cmake/android.toolchain.cmake -DANDROID_PLATFORM=30 -DANDROID=ON -DANDROID_ABI=armeabi-v7a -DJAVA_HOME=C:/Library/Runtime/jdk-1.8.0_291 -DANDROID_NATIVE_API_LEVEL=24 -DANDROID_BUILD_ABI_armeabi-v7a=ON -DANDROID_DEPLOY_QT:FILEPATH=C:/Qt/5.15.2/android/bin/androiddeployqt -DANDROID_SDK:PATH=C:/Users/xgd/AppData/Local/Android/Sdk -DANDROID_NDK:PATH=C:/Users/xgd/AppData/Local/Android/Sdk/ndk/21.4.7075529
```

# wasm build

## ubuntu host build for wasm platform

```shell
# play emscripten from source code is recommended
# firstly, build multi-threading enabled qt-5.15.2 wasm version
~/source/qt-5.15.2/./configure -xplatform wasm-emscripten -make examples -feature-thread -prefix ~/shared/Qt/5.15.2/wasm_32

source ~/.env/emcc

emcmake cmake .. -G "Ninja" -DQt5_DIR:PATH=~/shared/Qt/5.15.2/wasm_32/lib/cmake/Qt5 -DWASM=ON -DBUILD_SHARED_LIBS=OFF
```

## windows host build for wasm platform

```bat
# firstly, build multi-threading enabled qt-5.15.2 wasm version
D:/source/qt-5.15.2/configure -no-warnings-are-errors -xplatform wasm-emscripten -platform win32-g++ -make examples -feature-thread -prefix C:/Qt/5.15.2/wasm_32

mingw32-make -j 16 install

C:\Library\Runtime\emsdk-1.39.8\env.bat

emcmake cmake .. -G "Ninja" -DQt5_DIR:PATH=C:/Qt/5.15.2/wasm_32/lib/cmake/Qt5 -DWASM=ON -DBUILD_SHARED_LIBS=OFF
```

# a template for ~/.env/emcc

```shell
# binaryen, build it from source
export BINARYEN_DIR=~/shared/binaryen
# jdk, download from oracle
export JAVA_HOME=~/shared/jdk1.8.0_301
# qt5, download from mirrors
export Qt5_DIR=~/shared/Qt/5.15.2/wasm_32
export LD_LIBRARY_PATH=$Qt5_DIR/lib:$LD_LIBRARY_PATH
# python, install with conda
conda activate py27
# llvm, build it from source
export LLVM_INSTALL_DIR=~/shared/llvm
export LD_LIBRARY_PATH=$LLVM_INSTALL_DIR/lib:$LD_LIBRARY_PATH
# emcc, checkout it from github
WORK_DIR=$(pwd)
cd ~/git/emscripten && git checkout 1.39.8
cd $WORK_DIR
export EMSDK_DIR=~/git/emscripten
# full path
export PATH=$EMSDK_DIR:$LLVM_INSTALL_DIR/bin:$Qt5_DIR/bin:$JAVA_HOME/bin:$BINARYEN_DIR/bin:$PATH
```

# a template for windows .emscripten

```txt
NODE_JS = 'C:/Library/Runtime/node/node.exe'
PYTHON = 'C:/Library/Runtime/python-3.8/python.exe'
JAVA = 'C:/Library/Runtime/jdk-1.8.0_291/bin/java.exe'
LLVM_ROOT = 'C:/Library/Runtime/emsdk-1.39.8/bin'
BINARYEN_ROOT = 'C:/Library/Runtime/emsdk-1.39.8'
EMSCRIPTEN_ROOT = 'C:/Library/Runtime/emsdk-1.39.8/emscripten'
COMPILER_ENGINE = NODE_JS
JS_ENGINES = [NODE_JS]
```

# cmake build

```shell
cmake --build . -j $(nproc) --config Release --target install
```

# build opencv(_contrib)

## ubuntu host build for gcc, with vtk, qt and cuda

```shell
cmake -G "Ninja" ~/git/opencv -DCMAKE_INSTALL_PREFIX=~/shared/opencv4 -DOPENCV_EXTRA_MODULES_PATH=~/git/opencv_contrib/modules -DVTK_DIR=~/shared/vtk/lib/cmake/vtk-9.0 -DWITH_CUDA=ON -DOPENCV_DNN_CUDA=ON -DWITH_QT=ON -DWITH_OPENGL=ON -DWITH_VTK=ON -DBUILD_opencv_js=OFF -DBUILD_opencv_js_bindings_generator=OFF -DBUILD_JAVA=OFF -DBUILD_opencv_java_bindings_generator=OFF -DBUILD_opencv_python_bindings_generator=OFF -DBUILD_opencv_python_tests=OFF -DBUILD_TESTS=OFF -DBUILD_PERF_TESTS=OFF -DBUILD_SHARED_LIBS=ON -DINSTALL_PDB_COMPONENT_EXCLUDE_FROM_ALL=OFF -DBUILD_DOCS=OFF -DCMAKE_BUILD_TYPE=Release
```

# build libtorch

## ubuntu host build for gcc

```shell
sudo apt-get install python3-typing-extensions python3-pretty-yaml libmkldnn-dev liblapack-dev
git clone --recursive https://github.com/pytorch/pytorch
git submodule sync
git submodule update --init --recursive

cmake -G "Ninja" ~/git/pytorch -DCMAKE_INSTALL_PREFIX=~/shared/libtorch -DOpenCV_DIR=~/shared/opencv4/lib/cmake/opencv4 -DBUILD_PYTHON=OFF -DUSE_OPENCV=ON -DUSE_DISTRIBUTED=OFF -DUSE_TENSORPIPE=OFF -DUSE_CUDA=ON -DUSE_MKLDNN=ON -DUSE_NCCL=OFF -DUSE_NUMPY=OFF -DCMAKE_BUILD_TYPE=Release -DBUILD_DOCS=ON -DCMAKE_INSTALL_DOCDIR=~/shared/libtorch/doc
```

### windows host build for msvc

```shell
set SHARED_LIB_DIR=C:/Library/Shared
# debug
cmake D:/git/pytorch -G "Ninja" -DCMAKE_INSTALL_PREFIX=%SHARED_LIB_DIR%/libtorch -DBUILD_PYTHON=OFF -DUSE_OPENCV=ON -DOpenCV_DIR=%SHARED_LIB_DIR%/opencv4/x64/vc16/lib -DUSE_DISTRIBUTED=OFF -DUSE_TENSORPIPE=OFF -DUSE_CUDA=ON -DUSE_MKLDNN=OFF -DUSE_NCCL=OFF -DUSE_NUMPY=OFF -DUSE_MKLDNN=ON -DCMAKE_BUILD_TYPE=Debug
# release
cmake D:/git/pytorch -G "Ninja" -DCMAKE_INSTALL_PREFIX=%SHARED_LIB_DIR%/libtorch -DBUILD_PYTHON=OFF -DUSE_OPENCV=ON -DOpenCV_DIR=%SHARED_LIB_DIR%/opencv4/x64/vc16/lib -DUSE_DISTRIBUTED=OFF -DUSE_TENSORPIPE=OFF -DBUILD_TEST=ON -DUSE_CUDA=ON -DUSE_MKLDNN=OFF -DUSE_NCCL=OFF -DUSE_MKLDNN=ON  -DUSE_NUMPY=OFF -DCMAKE_BUILD_TYPE=Release -DBUILD_DOCS=ON -DCMAKE_INSTALL_DOCDIR=%SHARED_LIB_DIR%/libtorch/doc
```
