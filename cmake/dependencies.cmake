# This file contains all required packages for the project

find_package(PulseAudio REQUIRED)
find_package(yaml-cpp REQUIRED)

find_package(Boost REQUIRED
    COMPONENTS
        filesystem
        program_options
        system
)
