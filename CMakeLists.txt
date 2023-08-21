cmake_minimum_required(VERSION 3.22)

project(COCR VERSION 1.0.1 LANGUAGES C CXX)

set(XGD_THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party)

include(cmake/cmake-utils.cmake)
include(cmake/project-config.cmake)

function(xgd_build_third_party)
    set(XGD_THIRD_PARTY_LIBS
            abseil ade avalontoolkit
            benchmark boost
            cairo coordgen cpufeatures cutlass
            eigen
            freesasa freetype
            ggml gtest
            inchi indigo
            lbfgs libpng libxml2
            maeparser
            ncnn nodeeditor
            openbabel opencv openssl
            pixman protobuf
            rapidjson rdkit ringdecomposerlib rxcpp
            spdlog
            taskflow tinyxml2
            yaehmop yoga
            zlib zstd)
    foreach (XGD_THIRD_PARTY_LIB ${XGD_THIRD_PARTY_LIBS})
        include(cmake/build-${XGD_THIRD_PARTY_LIB}.cmake)
    endforeach ()
endfunction()

xgd_build_third_party()

add_subdirectory(apps)
add_subdirectory(lib)
