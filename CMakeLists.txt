cmake_minimum_required(VERSION 3.20)
project(light_profile_project LANGUAGES CXX)

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

add_subdirectory(lib/light_core)

add_library(light_profile
        lib/light_profile/src/MiniJson.cpp
        lib/light_profile/src/LightProfileLoader.cpp
        lib/light_profile/src/LightAnimator.cpp
        lib/light_profile/src/LightController.cpp
)
target_include_directories(light_profile PUBLIC
        lib/light_profile/include
)
target_link_libraries(light_profile PUBLIC light_core)
target_compile_features(light_profile PUBLIC cxx_std_17)

add_executable(light_console_demo
        tools/console_demo/main.cpp
)
target_link_libraries(light_console_demo PRIVATE light_profile)
target_compile_features(light_console_demo PRIVATE cxx_std_17)

add_executable(desktop_light_sim
        tools/desktop_sim/src/main.cpp
        lib/light_core/src/LayoutData.cpp
        lib/light_core/include/light/LayoutData.h
)
target_link_libraries(desktop_light_sim PRIVATE
        light_profile
        SDL2::SDL2main
        SDL2::SDL2
        SDL2_ttf::SDL2_ttf
)
target_compile_features(desktop_light_sim PRIVATE cxx_std_17)

add_custom_command(TARGET light_console_demo POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:light_console_demo>/assets"
)

add_custom_command(TARGET desktop_light_sim POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:desktop_light_sim>/assets"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "C:/msys64/ucrt64/bin/SDL2.dll"
        "$<TARGET_FILE_DIR:desktop_light_sim>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "C:/msys64/ucrt64/bin/SDL2_ttf.dll"
        "$<TARGET_FILE_DIR:desktop_light_sim>"
)