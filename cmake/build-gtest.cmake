# gtest
set(ROOT_DIR ${XGD_THIRD_PARTY_DIR}/gtest-src/gtest)
set(GTEST_INC_DIR ${ROOT_DIR}/googletest/include)
set(GTEST_SRC_DIR ${ROOT_DIR}/googletest/src)
set(GMOCK_INC_DIR ${ROOT_DIR}/googlemock/include)
set(GMOCK_SRC_DIR ${ROOT_DIR}/googlemock/src)
xgd_add_library(
        gtest
        SRC_FILES ${GTEST_SRC_DIR}/gtest-all.cc ${GMOCK_SRC_DIR}/gmock-all.cc
        INCLUDE_DIRS ${GTEST_INC_DIR} ${GMOCK_INC_DIR}
        PRIVATE_INCLUDE_DIRS ${GTEST_SRC_DIR}/.. ${GMOCK_SRC_DIR}/..
)
if (BUILD_SHARED_LIBS)
    target_compile_definitions(gtest PRIVATE "GTEST_CREATE_SHARED_LIBRARY=1")
endif ()
xgd_link_threads(gtest)
xgd_add_library(
        gtest_main
        STATIC
        SRC_FILES ${GTEST_SRC_DIR}/gtest_main.cc
)
xgd_link_gtest(gtest_main GTEST DONT_ADD_TEST)
xgd_add_library(
        gmock_main
        STATIC
        SRC_FILES ${GMOCK_SRC_DIR}/gmock_main.cc
)
xgd_link_gtest(gmock_main GTEST DONT_ADD_TEST)
