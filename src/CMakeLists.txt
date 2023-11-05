add_executable(${PROJECT_NAME}
    led.cpp
    log.cpp
    main.cpp
    pa_context.cpp
    pa_threaded_mainloop.cpp
    updater_default.cpp
    updater_named.cpp
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        ${Boost_LIBRARIES}
        pulse
        yaml-cpp
)

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        CONFIG_FILE=\"${CONFIG_FILE}\"
)

install(TARGETS ${PROJECT_NAME})
