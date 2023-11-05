set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
"Daemon synchronizing mute status of PulseAudio with LED indicators\n\
LED indication of audio mute status is a common case for IBM/Lenovo ThinkPad or Huawei laptops.\n\
This service runs in the background and synchronizes mute status of PulseAudio with LED indicators, making the indication consistent with the actual playback or recording devices being in use, whether it is Bluetooth, USB, or a virtual device.\n\
pulseaudio-led-mute supports native PulseAudio server or the one implemented by PipeWire.")
set(CPACK_DEBIAN_PACKAGE_SECTION sound)

set(CPACK_PACKAGE_VENDOR "Aleksei Ilin")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER ${CPACK_PACKAGE_VENDOR})
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE
    "https://github.com/a-ilin/pulseaudio-led-mute/")

set(CPACK_VERBATIM_VARIABLES YES)

set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})

set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE)
set(CPACK_RESOURCE_FILE_README ${CMAKE_CURRENT_SOURCE_DIR}/README.md)

set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_COMPONENTS_GROUPING ALL_COMPONENTS_IN_ONE)
set(CPACK_DEB_COMPONENT_INSTALL YES)

set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_STRIP_FILES YES)

# Configure deb scripts
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/deb/preinst.in
               ${CMAKE_CURRENT_BINARY_DIR}/deb/preinst @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/deb/postinst.in
               ${CMAKE_CURRENT_BINARY_DIR}/deb/postinst @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/deb/prerm.in
               ${CMAKE_CURRENT_BINARY_DIR}/deb/prerm @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/deb/postrm.in
               ${CMAKE_CURRENT_BINARY_DIR}/deb/postrm @ONLY)

set(CPACK_DEBIAN_PACKAGE_CONTROL_STRICT_PERMISSION TRUE)
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA
    ${CMAKE_CURRENT_BINARY_DIR}/deb/preinst
    ${CMAKE_CURRENT_BINARY_DIR}/deb/postinst
    ${CMAKE_CURRENT_BINARY_DIR}/deb/prerm
    ${CMAKE_CURRENT_BINARY_DIR}/deb/postrm)

# Paths
set(CPACK_PACKAGING_INSTALL_PREFIX /)

include(CPack)
