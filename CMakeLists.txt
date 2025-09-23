cmake_minimum_required(VERSION 3.10)
project(ReactorApplication LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_library(SDL2_GFX_LIB SDL2_gfx)
find_path(SDL2_GFX_INCLUDE SDL2/SDL2_gfxPrimitives.h)

add_subdirectory(libs/geometry_module)
add_subdirectory(libs/MyGUI)
add_subdirectory(libs/ReactorModel)

add_executable(ReactorApplication
    main.cpp
)

target_include_directories(ReactorApplication
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_compile_options(ReactorApplication PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
target_link_options(ReactorApplication PRIVATE -fsanitize=address,undefined)

target_include_directories(ReactorApplication PRIVATE ${SDL2_GFX_INCLUDE})
target_link_libraries(ReactorApplication PRIVATE ${SDL2_GFX_LIB})

target_link_libraries(ReactorApplication PRIVATE
    geometry_module
    MyGUI
    ReactorModel
)   