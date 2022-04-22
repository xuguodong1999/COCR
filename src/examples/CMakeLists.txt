project(examples LANGUAGES CXX)

include(${CMAKE_SOURCE_DIR}/cmake/Utils.cmake)

find_package(OpenCV QUIET)
find_package(Torch QUIET)
#find_package(Qt5 COMPONENTS Widgets QUIET)

CHECK_OPENCV(${PROJECT_NAME})
CHECK_LIBTORCH(${PROJECT_NAME})
#CHECK_QT(${PROJECT_NAME} "Widgets")

function(addNNExecutable CPP_NAME)
    addExecutable(${CPP_NAME} ${CPP_NAME}.cpp)
    addLibraryDeps(${CPP_NAME} els_nn)
    linkLibtorch(${CPP_NAME})
    linkOpenCV(${CPP_NAME})
    target_compile_definitions(${CPP_NAME} PRIVATE
            "DATASET_PATH=\"${DEV_ASSETS_DIR}/datasets\"")
endfunction()

addExecutable(test_header test_header.cpp)

addNNExecutable(autograd)
addNNExecutable(autograd2)

addExecutable(bgl_example bgl_example.cpp)
linkOpenBabel(bgl_example)
linkBoost(bgl_example)

addNNExecutable(conv_forward)
addNNExecutable(crnn)
addNNExecutable(custom_dataset)
addNNExecutable(data_loader_compare)
addNNExecutable(dcgan)

addExecutable(eigen3_cn_doc_ch1 eigen3_cn_doc_ch1.cpp)
addExecutable(eigen3_cn_doc_ch2 eigen3_cn_doc_ch2.cpp)
addExecutable(eigen3_cn_doc_ch3 eigen3_cn_doc_ch3.cpp)
linkQt(eigen3_cn_doc_ch3 "Gui")
addExecutable(eigen3_cn_doc_ch4 eigen3_cn_doc_ch4.cpp)

addNNExecutable(gan)

addExecutable(hello_eigen3 hello_eigen3.cpp)
addExecutable(hello_fdeep hello_fdeep.cpp)
addExecutable(hello_fmt hello_fmt.cpp)
addExecutable(hello_rx hello_rx.cpp)

addNNExecutable(hello_torch)
addNNExecutable(linear_regression)
addNNExecutable(linear_regression2)
addNNExecutable(logistic_regression)
addNNExecutable(molecular_gnn_smiles)
addNNExecutable(mnist)
addNNExecutable(tensors)
addNNExecutable(train_cifar)
#addNNExecutable(train_couch)
#addNNExecutable(train_object365)
addNNExecutable(tutorials)
addNNExecutable(yolov4)

