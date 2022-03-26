# wasm build

## ubuntu host build for wasm platform

```shell
# play emscripten from source code is recommended
# firstly, build multi-threading enabled qt-5.15.2 wasm version
~/source/qt-5.15.2/./configure \
    -xplatform wasm-emscripten \
    -make examples \
    -feature-thread \
    -prefix ~/shared/Qt/5.15.2/wasm_32

source ~/.env/emcc

emcmake cmake .. -G "Ninja" \
    -DQt5_DIR:PATH=~/shared/Qt/5.15.2/wasm_32/lib/cmake/Qt5 \
    -DWASM=ON \
    -DBUILD_SHARED_LIBS=OFF \
    -Wno-dev
```

## windows host build for wasm platform

```shell
# firstly, build multi-threading enabled qt-5.15.2 wasm version
D:/source/qt-5.15.2/configure ^
    -no-warnings-are-errors ^
    -xplatform wasm-emscripten ^
    -platform win32-g++ ^
    -make examples ^
    -feature-thread ^
    -prefix C:/Qt/5.15.2/wasm_32

mingw32-make -j 16 install

C:\Library\Runtime\emsdk-1.39.8\env.bat

emcmake cmake .. -G "Ninja" ^
    -DQt5_DIR:PATH=C:/Qt/5.15.2/wasm_32/lib/cmake/Qt5 ^
    -DWASM=ON ^
    -DBUILD_SHARED_LIBS=OFF ^
    -Wno-dev
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

```py
NODE_JS = 'C:/Library/Runtime/node/node.exe'
PYTHON = 'C:/Library/Runtime/python-3.8/python.exe'
JAVA = 'C:/Library/Runtime/jdk-1.8.0_291/bin/java.exe'
LLVM_ROOT = 'C:/Library/Runtime/emsdk-1.39.8/bin'
BINARYEN_ROOT = 'C:/Library/Runtime/emsdk-1.39.8'
EMSCRIPTEN_ROOT = 'C:/Library/Runtime/emsdk-1.39.8/emscripten'
COMPILER_ENGINE = NODE_JS
JS_ENGINES = [NODE_JS]
```