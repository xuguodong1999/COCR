# yoga
set(ROOT_DIR ${XGD_THIRD_PARTY_DIR}/yoga-src/yoga)
set(INC_DIR ${ROOT_DIR})
set(SRC_DIR ${ROOT_DIR}/yoga)
xgd_add_library(
        yoga
        SRC_DIRS
        ${SRC_DIR}
        ${SRC_DIR}/internal
        ${SRC_DIR}/event
        INCLUDE_DIRS ${INC_DIR}
)
if (WIN32 AND BUILD_SHARED_LIBS)
    target_compile_definitions(yoga PRIVATE _WINDLL)
endif ()
target_link_libraries(yoga PRIVATE $<$<BOOL:${ANDROID}>:log>)
