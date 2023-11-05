# Application config
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/pulseaudio-led-mute.yml
        DESTINATION ${CMAKE_INSTALL_SYSCONFDIR})

# systemd service
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/pulseaudio-led-mute.service.in
               ${CMAKE_CURRENT_BINARY_DIR}/pulseaudio-led-mute.service @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pulseaudio-led-mute.service
        DESTINATION ${SYSTEMD_SERVICE_PATH}/user)

# udev rules
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/99-led-audio.rules
        DESTINATION ${UDEV_RULES_PATH})
