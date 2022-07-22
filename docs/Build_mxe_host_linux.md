# Build with MXE

## install mxe

`make MXE_TARGETS="x86_64-w64-mingw32.static qt5"`

## build

```shell

export PATH=~/git/mxe/usr/bin:$PATH

unset `env | \
 grep -vi '^EDITOR=\|^HOME=\|^LANG=\|MXE\|^PATH=' | \
 grep -vi 'PKG_CONFIG\|PROXY\|^PS1=\|^TERM=' | \
 cut -d '=' -f1 | tr '\n' ' '`

mkdir opencv4-build && cd opencv4-build

x86_64-w64-mingw32.static-cmake -G "Ninja" ~/git/cpp-dev/opencv -DCMAKE_INSTALL_PREFIX=../opencv4 -DBUILD_opencv_js=OFF -DWITH_QT=OFF -DWITH_VTK=OFF -DBUILD_opencv_js_bindings_generator=OFF -DBUILD_JAVA=OFF -DBUILD_opencv_java_bindings_generator=OFF -DBUILD_opencv_python_bindings_generator=OFF -DBUILD_opencv_python_tests=OFF -DBUILD_TESTS=OFF -DBUILD_PERF_TESTS=OFF -DBUILD_SHARED_LIBS=OFF -DINSTALL_PDB_COMPONENT_EXCLUDE_FROM_ALL=OFF -DBUILD_DOCS=OFF -DCMAKE_BUILD_TYPE=Release

cd ..

x86_64-w64-mingw32.static-cmake -G "Ninja" .. -DOpenCV_DIR:PATH=path/to/opencv4/lib/cmake/opencv4 -DBUILD_SHARED_LIBS=OFF 

x86_64-w64-mingw32.static-cmake --build . --parallel --config Release --target leafxy

```
