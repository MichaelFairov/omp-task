# File: CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
project(userspace_logger VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source)

# Shared library
add_library(preload_logger SHARED
    source/preload_logger.cpp
    source/shared_buffer.cpp
    source/log_entry.cpp
)

target_link_libraries(preload_logger dl pthread)

# Daemon executable
add_executable(logger_daemon
    source/daemon_logger.cpp
    source/shared_buffer.cpp
    source/log_entry.cpp
)

target_link_libraries(logger_daemon pthread)

# Install targets
install(TARGETS preload_logger DESTINATION lib)
install(TARGETS logger_daemon DESTINATION bin)
install(DIRECTORY source/ DESTINATION include FILES_MATCHING PATTERN "*.hpp")

# Packaging
include(CPack)
set(CPACK_PACKAGE_NAME "userspace-logger")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_CONTACT "dev@example.com")
set(CPACK_GENERATOR "RPM")
set(CPACK_PACKAGING_INSTALL_PREFIX "/usr")
set(CPACK_RPM_PACKAGE_LICENSE "MIT")
set(CPACK_RPM_PACKAGE_RELEASE 1)
set(CPACK_RPM_PACKAGE_GROUP "Development/Debug")
set(CPACK_RPM_PACKAGE_URL "https://example.com")

