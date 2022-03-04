include(${CMAKE_SOURCE_DIR}/cmake/Utils.cmake)
findQtModules()

# copy from ncnn
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s USE_PTHREADS=1 -s PTHREAD_POOL_SIZE=4 -s TOTAL_MEMORY=1GB")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_PTHREADS=1 -s PTHREAD_POOL_SIZE=4 -s TOTAL_MEMORY=1GB")
# copy from qmake-generated Makefile
set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -s WASM=1 -s FULL_ES2=1 -s FULL_ES3=1 -s USE_WEBGL2=1 -s EXIT_RUNTIME=1 -s ERROR_ON_UNDEFINED_SYMBOLS=1 -s EXTRA_EXPORTED_RUNTIME_METHODS=[\"UTF16ToString\",\"stringToUTF16\"] --bind -s FETCH=1 -O2 -s USE_PTHREADS=1 -s PTHREAD_POOL_SIZE=4 -s TOTAL_MEMORY=1GB")

include(FetchContent)
FetchContent_Declare(ncnn_android
        URL https://github.com/Tencent/ncnn/releases/download/20210720/ncnn-20210720-webassembly.zip)
FetchContent_MakeAvailable(ncnn_android)
FetchContent_Declare(opencv_android
        URL https://github.com/nihui/opencv-mobile/releases/download/v14/opencv-mobile-4.5.4-webassembly.zip)
FetchContent_MakeAvailable(opencv_android)
set(ncnn_DIR ${ncnn_android_SOURCE_DIR}/threads/lib/cmake/ncnn)
set(OpenCV_DIR ${opencv_android_SOURCE_DIR}/threads/lib/cmake/opencv4)
