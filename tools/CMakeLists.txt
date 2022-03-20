add_subdirectory(app_qwidget)

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
find_package(OpenCV REQUIRED)

add_executable(data_gen data_gen.cpp)
add_dependencies(data_gen cocr::data)
target_link_libraries(data_gen PRIVATE cocr::data cocr::stroke cocr::base Qt5::Widgets opencv_highgui)
target_include_directories(data_gen PRIVATE ${cocr_INCLUDE_DIR})