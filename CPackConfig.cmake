include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_NAME "userspace-logger")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_CONTACT "dev@example.com")
set(CPACK_GENERATOR "RPM")
set(CPACK_PACKAGING_INSTALL_PREFIX "/usr")

set(CPACK_RPM_PACKAGE_LICENSE "MIT")
set(CPACK_RPM_PACKAGE_RELEASE 1)
set(CPACK_RPM_PACKAGE_GROUP "Development/Debug")
set(CPACK_RPM_PACKAGE_URL "https://example.com")
set(CPACK_RPM_CHANGELOG_FILE "${CMAKE_SOURCE_DIR}/ChangeLog")

include(CPack)
