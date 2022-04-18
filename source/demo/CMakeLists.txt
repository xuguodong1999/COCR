project(standalone LANGUAGES CXX)

include(${CMAKE_SOURCE_DIR}/cmake/Utils.cmake)

find_package(OpenCV QUIET)
find_package(Torch QUIET)
find_package(Qt5 COMPONENTS Widgets QUIET)

CHECK_OPENCV(${PROJECT_NAME})
CHECK_LIBTORCH(${PROJECT_NAME})
CHECK_QT(${PROJECT_NAME} "Widgets")

addExecutable(c2fmt c2fmt.cpp)

addExecutable(data_loader_compare data_loader_compare.cpp)
linkLibtorch(data_loader_compare)

addExecutable(fix_js_import fix_js_import.cpp)
linkQt(fix_js_import "Core")

addExecutable(hello_gemm hello_gemm.cpp)

addExecutable(hello_world hello_world.cpp)
linkLibtorch(hello_world)

addExecutable(molecular_gnn_smiles molecular_gnn_smiles.cpp)
linkLibtorch(molecular_gnn_smiles)

addExecutable(simplify_torch_import simplify_torch_import.cpp)
linkQt(simplify_torch_import "Core")

function(addNNExecutable CPP_NAME)
    addExecutable(${CPP_NAME} ${CPP_NAME}.cpp)
    addLibraryDeps(${CPP_NAME} els_nn)
    linkLibtorch(${CPP_NAME})
    target_compile_definitions(${CPP_NAME} PRIVATE
            "DATASET_PATH=\"$ENV{HOME}/datasets\"")
endfunction()

addNNExecutable(demo_crnn)
addNNExecutable(demo_train_cifar)
#addNNExecutable(demo_train_couch)
#addNNExecutable(demo_train_object365)
addNNExecutable(demo_yolov4)
