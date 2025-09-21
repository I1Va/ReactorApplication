cmake_minimum_required(VERSION 3.10)
project(ReactorApplication LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(libs/geometry_module)
add_subdirectory(libs/MyGUI)
add_subdirectory(libs/ReactorModel)

add_executable(ReactorApplication
    main.cpp
)

target_include_directories(ReactorApplication
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(ReactorApplication PRIVATE
    geometry_module
    MyGUI
    ReactorModel
)   