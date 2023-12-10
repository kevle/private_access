cmake_minimum_required(VERSION 3.16)

project(private_access CXX)

add_library(private_access INTERFACE)
target_include_directories(private_access INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

install(TARGETS private_access 
        EXPORT private_access_export
        INCLUDES DESTINATION "include"
)
install(DIRECTORY include DESTINATION .)
install(EXPORT private_access_export
    FILE private_access.cmake
    NAMESPACE private_access::
    DESTINATION lib/cmake/private_access)

enable_testing()
add_subdirectory(test)

