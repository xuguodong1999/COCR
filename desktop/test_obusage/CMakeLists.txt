project(cocr-test_obusage)


set(SRC_PRO_FILES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

set(SRC_ALL ${RES_FILES} ${BIG_RES_FILES} ${SRC_PRO_FILES}
        ${UI_SRC} ${MOL_SRC} ${HW_SRC} ${BASE_SRC})

if (ALLOW_USER_TRAIN)
    list(APPEND SRC_ALL ${TORCH_SRC})
endif (ALLOW_USER_TRAIN)

add_executable(${PROJECT_NAME} ${SRC_ALL})

target_compile_definitions(${PROJECT_NAME} PRIVATE
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

if (MSVC)
    set(MSVC_LIBS Qt5::Widgets Qt5::Quick Qt5::3DAnimation Qt5::3DCore
            Qt5::3DExtras Qt5::3DInput Qt5::3DLogic Qt5::3DRender
            openbabel coordgen)
    if (ALLOW_USER_TRAIN)
        list(APPEND MSVC_LIBS ${TORCH_LIBRARIES})
        list(APPEND MSVC_LIBS ${OpenCV_LIBS})
    else (ALLOW_USER_TRAIN)
        list(APPEND MSVC_LIBS opencv_core opencv_highgui
                opencv_imgproc opencv_imgcodecs opencv_dnn)
    endif (ALLOW_USER_TRAIN)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${MSVC_LIBS})
elseif (UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${COCR_UNIX_3RDPARTY_LIB})
endif (MSVC)
