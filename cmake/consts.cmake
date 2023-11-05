include(GNUInstallDirs)

# By default install to /usr
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX / CACHE PATH "" FORCE)
endif()

set(CMAKE_INSTALL_BINDIR usr/bin)
set(CMAKE_INSTALL_DOCDIR usr/share/doc)
set(CMAKE_INSTALL_MANDIR usr/share/man)

# Program configuration
set(CONFIG_FILE
    /${CMAKE_INSTALL_SYSCONFDIR}/pulseaudio-led-mute.yml
    CACHE STRING "Program configuration file")

# udev rules
set(UDEV_RULES_PATH
    ${CMAKE_INSTALL_SYSCONFDIR}/udev/rules.d
    CACHE STRING "Target directory for udev rule installation")

# systemd service
set(SYSTEMD_SERVICE_PATH
    ${CMAKE_INSTALL_LIBDIR}/systemd
    CACHE STRING "Target directory for systemd service installation")
