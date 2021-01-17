project(qdarknet)

cmake_policy(SET CMP0104 OLD)
# for cudnn
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/Modules/" ${CMAKE_MODULE_PATH})

enable_language(CUDA)
find_package(CUDA REQUIRED)
set(CUDA_ARCHITECTURES Common)
cuda_select_nvcc_arch_flags(CUDA_ARCH_FLAGS ${CUDA_ARCHITECTURES})

set(CUDNN_INCLUDE_DIR /usr/include)
set(CUDNN_LIBRARY /usr/lib/x86_64-linux-gnu/libcudnn.so)
find_package(CUDNN)
find_package(Threads REQUIRED)

file(GLOB HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
file(GLOB C_SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
file(GLOB CUDA_SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cu")
file(GLOB CPP_SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

set_source_files_properties(${C_SRC} PROPERTIES LANGUAGE CXX)

add_executable(${PROJECT_NAME} ${C_SRC} ${HEADERS} ${CUDA_SRC} ${CPP_SRC}
        ${BASE_SRC}  ${HW_SRC})

# clear -fPIC flag for nvcc
#set_property(TARGET Qt5::Core PROPERTY INTERFACE_COMPILE_OPTIONS)

target_link_libraries(${PROJECT_NAME} PRIVATE
        ${COCR_UNIX_3RDPARTY_LIB}
        CuDNN::CuDNN curand cublas cuda Threads::Threads)
target_compile_definitions(${PROJECT_NAME} PRIVATE -DCUDNN -DCUDNN_HALF -DOPENCV -DGPU)

