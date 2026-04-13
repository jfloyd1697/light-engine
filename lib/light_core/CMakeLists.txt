add_library(light_core
    src/Color.cpp
    src/Effects.cpp
    src/Layout.cpp
    src/LightEngine.cpp
    src/LayoutData.cpp
)

target_include_directories(light_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/external # nlohmann/json.hpp
)
