add_executable(light_sim
    src/main.cpp
)

target_link_libraries(light_sim PRIVATE light_core)
