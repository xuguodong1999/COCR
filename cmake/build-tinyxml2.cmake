# tinyxml2
set(ROOT_DIR ${XGD_THIRD_PARTY_DIR}/tinyxml2-src/tinyxml2)
set(INC_DIR ${ROOT_DIR})
set(SRC_DIR ${ROOT_DIR})
xgd_add_library(
        tinyxml2
        SRC_FILES
        ${SRC_DIR}/tinyxml2.cpp
        INCLUDE_DIRS ${INC_DIR}
)
if (BUILD_SHARED_LIBS)
    target_compile_definitions(tinyxml2 PRIVATE TINYXML2_EXPORT)
endif ()
target_link_libraries(tinyxml2 PRIVATE $<$<BOOL:${ANDROID}>:log>)
add_library(tinyxml2::tinyxml2 ALIAS tinyxml2)
