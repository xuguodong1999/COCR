if (XGD_USE_QT)
    add_subdirectory(COCR)
    add_subdirectory(COCRDataGen)
endif ()

add_subdirectory(EquationBalanceSolver)

if (XGD_USE_QT)
    add_subdirectory(TestCOCR)
endif ()

add_subdirectory(TestSpdlogStream)
