cmake_minimum_required(VERSION 3.30)

set(PROJECT_VERSION "0.1.0")
project(DevGames 
    LANGUAGES C CXX
    VERSION ${PROJECT_VERSION}
    DESCRIPTION "Lightweight game engine for your terminal"
    HOMEPAGE_URL "https://github.com/CalculusGames/DevGames"
)
set(PROJECT_DOCUMENTATION "https://calculusgames.github.io/devgames/")

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(cmdfx REQUIRED)

# Sources
file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS include/*.h include/*.hpp)

## Library
add_library(devgameslib STATIC ${SOURCES} ${HEADERS})
target_include_directories(devgameslib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(devgameslib PUBLIC cmdfx::cmdfx)

### Documentation
option(DOCS_DEVGAMES "Build DevGames API documentation with Doxygen" ON)

if (DOCS_DEVGAMES)
    find_package(Doxygen)

    if (DOXYGEN_FOUND)
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/build/Doxyfile.in)
    
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    
        add_custom_target(docs
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/docs
            COMMENT "Generating DevGames API documentation with Doxygen"
            VERBATIM)
    else()
        add_custom_target(docs
            COMMAND ${CMAKE_COMMAND} -E echo "Doxygen not found, skipping documentation generation"
            VERBATIM)
    endif()
endif()

## Executable
add_executable(devgames ${SOURCES} ${HEADERS})
target_include_directories(devgames
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(devgames PRIVATE devgameslib cmdfx::cmdfx)