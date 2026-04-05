cmake_minimum_required(VERSION 3.20)
project(light-engine LANGUAGES CXX)

option(LIGHT_BUILD_DESKTOP "Build desktop simulator" ON)

add_library(light_core
        lib/light_core/src/LightEngine.cpp
        lib/light_core/src/Layout.cpp
        lib/light_core/src/Effects.cpp
)

target_include_directories(light_core PUBLIC
        lib/light_core/include
)

target_compile_features(light_core PUBLIC cxx_std_17)

if(LIGHT_BUILD_DESKTOP)
    find_package(SDL2 REQUIRED)
    find_package(SDL2_ttf REQUIRED)

    add_executable(desktop_light_sim
            tools/desktop_sim/src/main.cpp
    )

    target_link_libraries(desktop_light_sim PRIVATE
            light_core
            SDL2::SDL2main
            SDL2::SDL2
            SDL2_ttf::SDL2_ttf
    )

    add_custom_command(TARGET desktop_light_sim POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "C:/msys64/ucrt64/bin/SDL2.dll"
            "$<TARGET_FILE_DIR:desktop_light_sim>"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "C:/msys64/ucrt64/bin/SDL2_ttf.dll"
            "$<TARGET_FILE_DIR:desktop_light_sim>"
    )
endif()