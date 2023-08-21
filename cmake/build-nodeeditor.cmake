# nodeeditor
set(ROOT_DIR ${XGD_THIRD_PARTY_DIR}/nodeeditor-src/nodeeditor)
set(INC_DIR ${ROOT_DIR}/include)
set(RES_DIR ${ROOT_DIR}/resources)
xgd_add_library(
        QtNodes
        SRC_DIRS ${INC_DIR}/QtNodes/internal
        SRC_FILES ${RES_DIR}/resources.qrc
        INCLUDE_DIRS ${INC_DIR}
        PRIVATE_INCLUDE_DIRS ${INC_DIR}/QtNodes/internal
)
xgd_link_qt(QtNodes PUBLIC Core Gui Widgets PRIVATE OpenGL)
if (BUILD_SHARED_LIBS)
    target_compile_definitions(QtNodes PUBLIC NODE_EDITOR_SHARED)
else ()
    target_compile_definitions(QtNodes PUBLIC NODE_EDITOR_STATIC)
endif ()
target_compile_definitions(QtNodes PRIVATE NODE_EDITOR_EXPORTS)
