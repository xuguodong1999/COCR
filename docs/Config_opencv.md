# build opencv(_contrib)

## ubuntu host build for gcc, with vtk, qt and cuda

```shell
cmake -G "Ninja" ~/git/opencv \
    -DCMAKE_INSTALL_PREFIX=~/shared/opencv4 \
    -DOPENCV_EXTRA_MODULES_PATH=~/git/opencv_contrib/modules \
    -DVTK_DIR=~/shared/vtk/lib/cmake/vtk-9.0 \
    -DWITH_CUDA=ON \
    -DOPENCV_DNN_CUDA=ON \
    -DWITH_QT=ON \
    -DWITH_OPENGL=ON \
    -DWITH_VTK=ON \
    -DBUILD_opencv_js=OFF \
    -DBUILD_opencv_js_bindings_generator=OFF \
    -DBUILD_JAVA=OFF \
    -DBUILD_opencv_java_bindings_generator=OFF \
    -DBUILD_opencv_python_bindings_generator=OFF \
    -DBUILD_opencv_python_tests=OFF \
    -DBUILD_TESTS=OFF \
    -DBUILD_PERF_TESTS=OFF \
    -DBUILD_SHARED_LIBS=ON \
    -DINSTALL_PDB_COMPONENT_EXCLUDE_FROM_ALL=OFF \
    -DBUILD_DOCS=OFF \
    -DCMAKE_BUILD_TYPE=Release
```