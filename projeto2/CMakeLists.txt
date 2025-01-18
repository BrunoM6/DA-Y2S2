cmake_minimum_required(VERSION 3.26)
project(projDA2)

set(CMAKE_CXX_STANDARD 17)

# Debugging messages
message(STATUS "Source directory: ${CMAKE_SOURCE_DIR}")
message(STATUS "Binary directory: ${CMAKE_CURRENT_BINARY_DIR}")

add_executable(projDA2
        src/main.cpp
        src/Graph.hpp
        src/Graph.cpp
        src/Menu.hpp
        src/Menu.cpp
)

# Check and copy dataset directory
if(EXISTS "${CMAKE_SOURCE_DIR}/dataset")
    file(COPY "${CMAKE_SOURCE_DIR}/dataset" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
    message(STATUS "Dataset directory found and copied.")
else()
    message(FATAL_ERROR "Dataset directory not found: ${CMAKE_SOURCE_DIR}/dataset")
endif()

# Doxygen Build
find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(BUILD_DOC_DIR "${CMAKE_SOURCE_DIR}/docs/output")
    if(NOT EXISTS "${BUILD_DOC_DIR}")
        file(MAKE_DIRECTORY "${BUILD_DOC_DIR}")
    endif()

    set(DOXYGEN_IN "${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile")
    set(DOXYGEN_OUT "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile")
    configure_file("${DOXYGEN_IN}" "${DOXYGEN_OUT}" @ONLY)

    message("Doxygen build started")
    add_custom_target(Doxygen ALL
            COMMAND "${DOXYGEN_EXECUTABLE}" "${DOXYGEN_OUT}"
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM)
else()
    message("Doxygen needs to be installed to generate the documentation.")
endif()
