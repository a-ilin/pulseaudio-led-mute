# ReadMe
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../README.md
        DESTINATION ${CMAKE_INSTALL_DOCDIR}/pulseaudio-led-mute)

# License
install(
  FILES ${CMAKE_CURRENT_SOURCE_DIR}/../LICENSE
  DESTINATION ${CMAKE_INSTALL_DOCDIR}/pulseaudio-led-mute
  RENAME copyright)

# Man page
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/pulseaudio-led-mute.1.scd.in
               ${CMAKE_CURRENT_BINARY_DIR}/pulseaudio-led-mute.1.scd @ONLY)

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/pulseaudio-led-mute.1
  COMMAND scdoc < ${CMAKE_CURRENT_BINARY_DIR}/pulseaudio-led-mute.1.scd >
          ${CMAKE_CURRENT_BINARY_DIR}/pulseaudio-led-mute.1
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/pulseaudio-led-mute.1.scd
  VERBATIM)

add_custom_target(man ALL
                  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/pulseaudio-led-mute.1)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pulseaudio-led-mute.1
        DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
