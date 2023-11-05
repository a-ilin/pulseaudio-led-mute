cmake_minimum_required(VERSION 3.22)

project(pulseaudio-led-mute
    LANGUAGES CXX
    VERSION 1.0.0
)

# Add feature summary
include(FeatureSummary)

# Local cmake modules
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_SOURCE_DIR}/cmake
)

include(consts)
include(flags)
include(dependencies)
include(packing)

feature_summary(WHAT ALL)

add_subdirectory(config)
add_subdirectory(doc)
add_subdirectory(src)
