# patch, build and install for thirdparty libraries

## Build on VS 2019

### prepare environments
* install git, cmake, vs2019, qt

### setup environments
* open x64 Native Tools Command Prompt for VS 2019
* commands I use to build on my PC: 
```bat
REM !!! replace with your own path here
set COCR_ROOT=C:\Users\xgd\source\COCR_dev

set INSTALL_DIR=%COCR_ROOT%\win\install
set OPENBABEL_BUILD_DIR=%COCR_ROOT%\win\build\openbabel
set OPENBABEL_SRC_DIR=%COCR_ROOT%\3rdparty\openbabel3.1.1
set OPENCV_BUILD_DIR=%COCR_ROOT%\win\build\opencv
set OPENCV_SRC_DIR=%COCR_ROOT%\3rdparty\opencv
set COORDGEN2D_BUILD_DIR=%COCR_ROOT%\win\build\coordgenlibs
set COORDGEN2D_SRC_DIR=%COCR_ROOT%\3rdparty\coordgenlibs1.4.2
```

### build openbabel 3.1.1
* replace std::binary_function, so we can use openbabel headers in cpp17
* replave \*\.obf with \*\.dll, so plugin can be loaded on wondows
* disable script, tools, test and doc module
* disable some formats: xml, json and inchi.
* cmake-config I use to build on my PC: 
```bat
if not exist %OPENBABEL_SRC_DIR% (
unzip %COCR_ROOT%\3rdparty\openbabel3.1.1.zip -d %COCR_ROOT%\3rdparty
)

if not exist %OPENBABEL_BUILD_DIR% (
md %OPENBABEL_BUILD_DIR%
)

cd %OPENBABEL_BUILD_DIR%

cmake %OPENBABEL_SRC_DIR% -DBUILD_SHARED=ON -DWITH_STATIC_INCHI=OFF -DWITH_STATIC_LIBXML=OFF -DWITH_INCHI=OFF -DOPENBABEL_USE_SYSTEM_INCHI=OFF -DOB_USE_PREBUILT_BINARIES=OFF -DENABLE_OPENMP=OFF -DWITH_MAEPARSER=OFF -DWITH_COORDGEN=OFF -DWITH_JSON=OFF -DOPENBABEL_USE_SYSTEM_RAPIDJSON=OFF -DBUILD_GUI=OFF -DENABLE_TESTS=OFF -DCMAKE_INSTALL_PREFIX=%INSTALL_DIR% -DBUILD_SHARED_LIBS=ON

cmake --build . -j 6 --config Debug --target install && cmake --build . -j 6 --config Release --target install
```

### build opencv 4.5.0
* cmake-config I use to build on my PC: 
```bat
cd %OPENCV_SRC_DIR% && git checkout 4.5.0

if not exist %OPENCV_BUILD_DIR% (
md %OPENCV_BUILD_DIR%
)

cd %OPENCV_BUILD_DIR%

cmake %OPENCV_SRC_DIR% -DBUILD_opencv_python_tests=OFF -DBUILD_opencv_python_bindings_generator=OFF -DBUILD_JAVA=OFF -DBUILD_opencv_java_bindings_generator=OFF -DBUILD_TESTS=OFF -DBUILD_PREF_TESTS=OFF -DWITH_ADE=OFF -DWITH_FFMPEG=OFF -DWITH_IPP=OFF -DWITH_EIGEN=OFF -DBUILD_SHARED_LIBS=ON -DBUILD_opencv_world=OFF -DCMAKE_INSTALL_PREFIX=%INSTALL_DIR% -DINSTALL_PDB_COMPONENT_EXCLUDE_FROM_ALL=OFF

cmake --build . -j 6 --config Debug --target install && cmake --build . -j 6 --config Release --target install
```

### build coordgenlibs 1.4.2
* cmake-config I use to build on my PC: 
```bat
if not exist %COORDGEN2D_SRC_DIR% (
unzip %COCR_ROOT%\3rdparty\coordgenlibs1.4.2.zip -d %COCR_ROOT%\3rdparty
)

if not exist %COORDGEN2D_BUILD_DIR% (
md %COORDGEN2D_BUILD_DIR%
)

cd %COORDGEN2D_BUILD_DIR%

cmake %COORDGEN2D_SRC_DIR% -DCMAKE_INSTALL_PREFIX=%INSTALL_DIR% -DBUILD_SHARED_LIBS=ON

cmake --build . -j 6 --config Debug --target install && cmake --build . -j 6 --config Release --target install
```

## Install on VS 2019
* commands I use to install on my PC: 
```bat
move /Y %INSTALL_DIR%\include\openbabel3\openbabel %INSTALL_DIR%\include\
rd /s/ q %INSTALL_DIR%\include\openbabel3
rd /s/ q %INSTALL_DIR%\include\inchi
move /Y %INSTALL_DIR%\bin\cmake %INSTALL_DIR%\lib\
move /Y %INSTALL_DIR%\bin\*.lib %INSTALL_DIR%\lib\
move /Y %INSTALL_DIR%\x64\vc16\lib\*.lib %INSTALL_DIR%\lib\
move /Y %INSTALL_DIR%\x64\vc16\bin\*.dll %INSTALL_DIR%\bin\
move /Y %INSTALL_DIR%\x64\vc16\bin\*.pdb %INSTALL_DIR%\bin\
move /Y %COORDGEN2D_BUILD_DIR%\Debug\*.pdb %INSTALL_DIR%\bin\
move /Y %INSTALL_DIR%\bin\cmake\openbabel3 %INSTALL_DIR%\lib\cmake
rd /s /q %INSTALL_DIR%\bin\cmake
```
