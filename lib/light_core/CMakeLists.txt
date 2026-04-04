add_library(light_core
    src/Canvas2D.cpp
    src/Color.cpp
    src/Layout.cpp
    src/LightEngine.cpp
    src/Rasterizer.cpp
    src/effects/PulseBlobEffect.cpp
    src/effects/StripeScrollEffect.cpp
)

target_include_directories(light_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
