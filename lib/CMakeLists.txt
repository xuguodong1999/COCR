function(xgd_add_init_libraries TARGET)
    xgd_add_library(${TARGET}
            SRC_FILES
            Impl.cpp
            INCLUDE_DIRS
            ${CMAKE_CURRENT_SOURCE_DIR}/../../include)
    xgd_generate_export_header(${TARGET} ${TARGET} ".h")
endfunction()

add_subdirectory(Base)

add_subdirectory(Chem)

add_subdirectory(COCR)

add_subdirectory(Data)

add_subdirectory(HandWriting)

add_subdirectory(InitOpenBabel)

if (XGD_USE_QT)
    add_subdirectory(InitQt)
endif()

add_subdirectory(InitSpdlog)

add_subdirectory(Math)

if (XGD_USE_QT)
    add_subdirectory(Qt)
endif ()

add_subdirectory(SpdlogStream)

add_subdirectory(Vision)