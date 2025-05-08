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
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# cmdfx
find_package(cmdfx REQUIRED)

# CLI11
FetchContent_Declare(
    CLI11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG v2.5.0
)
FetchContent_MakeAvailable(CLI11)

# Sources
file(GLOB_RECURSE SOURCES src/main/*.cpp)

add_executable(devgames ${SOURCES} ${HEADERS})
target_include_directories(devgames
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(devgames PRIVATE CLI11::CLI11)
set_target_properties(devgames PROPERTIES OUTPUT_NAME game)
target_compile_definitions(devgames PRIVATE DEVGAMES_VERSION="${PROJECT_VERSION}")

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_link_options(devgames PRIVATE -pthread)
endif()

# Installation
option(INSTALL_DEVGAMES "Install ${PROJECT_NAME}" ON)

if(WIN32)
    if(CMAKE_SIZEOF_VOID_P MATCHES "8")
        set(CMAKE_INSTALL_PREFIX "C:/Program Files/${CMAKE_PROJECT_NAME}")
    else()
        set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/${CMAKE_PROJECT_NAME}")
    endif()
elseif(APPLE)
    set(CMAKE_INSTALL_PREFIX "/usr/local")
else()
    set(CMAKE_INSTALL_PREFIX "/usr")
endif()

if (INSTALL_DEVGAMES)
    include(GNUInstallDirs)
   
    install(TARGETS devgames
        EXPORT devgamesTargets
        COMPONENT main
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
    install(EXPORT devgamesTargets
        FILE ${PROJECT_NAME}Targets.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/${PROJECT_NAME}
        COMPONENT main
    )

    include(CMakePackageConfigHelpers)
    write_basic_package_version_file("${PROJECT_NAME}ConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
    )
    configure_package_config_file(
            "${PROJECT_SOURCE_DIR}/cmake/Config.cmake.in"
            "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
            INSTALL_DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake
    )

    install(FILES 
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake" 
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/${PROJECT_NAME}
        COMPONENT main
    )

    install(FILES
        "${PROJECT_SOURCE_DIR}/README.md"
        "${PROJECT_SOURCE_DIR}/INSTALLER_README.txt"
        "${PROJECT_SOURCE_DIR}/LICENSE"
        "${PROJECT_SOURCE_DIR}/LICENSE.rtf"
        DESTINATION ${CMAKE_INSTALL_DOCDIR}
        COMPONENT attribution
    )
endif()

# Packaging
option(PACKAGE_DEVGAMES "Package ${PROJECT_NAME}" ON)

if (PACKAGE_DEVGAMES)
    set(ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})

    if (ARCHITECTURE STREQUAL "AMD64" OR ARCHITECTURE STREQUAL "x86_64")
        set(ARCHITECTURE "x64")
    endif()

    if (ARCHITECTURE STREQUAL "aarch64")
        set(ARCHITECTURE "arm64")
    endif()

    set(CPACK_THREADS 0) # Use all available threads
    set(CPACK_PACKAGE_DIRECTORY "${CMAKE_BINARY_DIR}/package")
    set(CPACK_PACKAGE_CHECKSUM SHA256)

    if (DEFINED PROJECT_VERSION_SUFFIX)
        set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}-${PROJECT_VERSION_SUFFIX}")
    else()
        set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
    endif()

    set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
    set(CPACK_PACKAGE_DESCRIPTION ${PROJECT_DESCRIPTION})
    set(CPACK_PACKAGE_HOMEPAGE_URL ${PROJECT_HOMEPAGE_URL})
    set(CPACK_PACKAGE_INSTALL_DIRECTORY ${PROJECT_NAME})
    set(CPACK_PACKAGE_VENDOR "Gregory Mitchell")
    set(CPACK_PACKAGE_CONTACT "gmitch215@calcugames.xyz")

    set(CPACK_RESOURCE_FILE_README "${PROJECT_SOURCE_DIR}/INSTALLER_README.txt")
    set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")

    if (WIN32)
        set(CPACK_GENERATOR "ZIP;NSIS;TGZ;WIX")

        set(CPACK_SYSTEM_NAME "windows-${ARCHITECTURE}")

        # NSIS
        set(CPACK_NSIS_DISPLAY_NAME ${PROJECT_NAME})
        set(CPACK_NSIS_DISPLAY_NAME_SET ON)
        set(CPACK_NSIS_PACKAGE_NAME ${PROJECT_NAME})

        set(CPACK_NSIS_MENU_LINKS "GitHub" ${PROJECT_HOMEPAGE_URL})
        set(CPACK_NSIS_MODIFY_PATH ON)

        # WiX
        set(CPACK_WIX_VERSION 4)
        set(CPACK_WIX_LICENSE_RTF "${PROJECT_SOURCE_DIR}/LICENSE.rtf")

        set(CPACK_WIX_UPGRADE_GUID "42937a72-0b91-4a57-8f45-60d84df05e66")
        set(CPACK_WIX_CMAKE_PACKAGE_REGISTRY ${PROJECT_NAME})
    elseif(APPLE)
        set(CPACK_GENERATOR "TGZ;ZIP;productbuild")

        set(CPACK_PACKAGING_INSTALL_PREFIX "/usr/local")
        set(CPACK_SYSTEM_NAME "macOS-${ARCHITECTURE}")

        set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE.rtf")

        set(CPACK_PRODUCTBUILD_IDENTIFIER "xyz.calcugames.${PROJECT_NAME}")
    elseif(UNIX)
        set(CPACK_GENERATOR "TGZ;TXZ;DEB;RPM")

        set(CPACK_SYSTEM_NAME "linux-${ARCHITECTURE}")

        set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${CPACK_PACKAGE_VENDOR} <${CPACK_PACKAGE_CONTACT}>")

        set(CPACK_RPM_PACKAGE_LICENSE "MIT")
    endif()

    include(CPack)

    cpack_add_component(main DISPLAY_NAME "DevGames" DESCRIPTION "The primary executable for DevGames" REQUIRED ON)
    cpack_add_component(attribution DISPLAY_NAME "Attribution" DESCRIPTION "Attribution files for DevGames" REQUIRED ON)

    cpack_add_component_group(games DISPLAY_NAME "Built-In Games" DESCRIPTION "Built-In Games for DevGames" EXPANDED ON)
endif()

# Built-In Games
option(BUILD_GAMES "Build Built-In Games" ON)

if (BUILD_GAMES)
    set(BUILTIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/builtin")
    file(GLOB CHILD_DIRS RELATIVE ${BUILTIN_DIR} ${BUILTIN_DIR}/*)

    set(DEVGAMES_INSTALL_DIRECTORY "${CMAKE_INSTALL_LIBEXECDIR}/DevGames/builtin")
    add_compile_definitions(DEVGAMES_ROOT_INSTALL_DIR="${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBEXECDIR}/DevGames")
    add_compile_definitions(DEVGAMES_THIRD_PARTY_INSTALL_DIR="${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBEXECDIR}/DevGames/install")
    add_compile_definitions(DEVGAMES_BUILTIN_INSTALL_DIR="${CMAKE_INSTALL_PREFIX}/${DEVGAMES_INSTALL_DIRECTORY}")

    set(DEVGAMES_BUILTIN_LIST "")
    foreach(dir ${CHILD_DIRS})
        if(IS_DIRECTORY "${BUILTIN_DIR}/${dir}")
            set(DEVGAMES_BUILTIN_LIST "${DEVGAMES_BUILTIN_LIST},${dir}")
            set(TARGET_NAME "devgames-${dir}")
            set(SOURCE_DIR "${BUILTIN_DIR}/${dir}")

            file(GLOB_RECURSE GAME_SOURCES "${SOURCE_DIR}/*.cpp")

            add_executable(${TARGET_NAME} ${GAME_SOURCES})
            target_include_directories(${TARGET_NAME} PRIVATE ${SOURCE_DIR})
            target_link_libraries(${TARGET_NAME} PRIVATE cmdfx::cmdfx CLI11::CLI11)
            set_target_properties(${TARGET_NAME} PROPERTIES 
                OUTPUT_NAME ${dir} 
                RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/games/${dir}" 
            )

            if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
                target_link_options(${TARGET_NAME} PRIVATE -pthread)
            endif()

            add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                COMMENT "Generating checksums for '${TARGET_NAME}'"
                COMMAND ${CMAKE_COMMAND} -E md5sum "$<TARGET_FILE:${TARGET_NAME}>" > "$<TARGET_FILE:${TARGET_NAME}>.md5"
                COMMAND ${CMAKE_COMMAND} -E sha1sum "$<TARGET_FILE:${TARGET_NAME}>" > "$<TARGET_FILE:${TARGET_NAME}>.sha1"
                COMMAND ${CMAKE_COMMAND} -E sha256sum "$<TARGET_FILE:${TARGET_NAME}>" > "$<TARGET_FILE:${TARGET_NAME}>.sha256"
                COMMAND ${CMAKE_COMMAND} -E sha512sum "$<TARGET_FILE:${TARGET_NAME}>" > "$<TARGET_FILE:${TARGET_NAME}>.sha512"
                VERBATIM
            )

            if (INSTALL_DEVGAMES)
                install(TARGETS ${TARGET_NAME}
                    EXPORT devgamesTargets
                    COMPONENT ${dir}
                    RUNTIME DESTINATION ${DEVGAMES_INSTALL_DIRECTORY}/${dir}
                    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/${dir}
                    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/${dir}
                )

                install(FILES
                    "${SOURCE_DIR}/${dir}.properties"
                    "$<TARGET_FILE:${TARGET_NAME}>.md5"
                    "$<TARGET_FILE:${TARGET_NAME}>.sha1"
                    "$<TARGET_FILE:${TARGET_NAME}>.sha256"
                    "$<TARGET_FILE:${TARGET_NAME}>.sha512"
                    DESTINATION ${DEVGAMES_INSTALL_DIRECTORY}/${dir}
                    COMPONENT ${dir}
                )
            endif()

            if (PACKAGE_DEVGAMES)
                # capitalize every word
                string(REPLACE "_" " " temp "${dir}")
                string(REPLACE "-" " " temp "${temp}")
                separate_arguments(words UNIX_COMMAND "${temp}")

                set(COMPONENT_NAME "")
                foreach(word IN LISTS words)
                    if(NOT word STREQUAL "")
                        string(SUBSTRING "${word}" 0 1 first_char)
                        string(TOUPPER "${first_char}" first_char_upper)
                        string(SUBSTRING "${word}" 1 -1 rest)
                        set(cap_word "${first_char_upper}${rest}")
                        string(APPEND COMPONENT_NAME "${cap_word}")
                    endif()
                endforeach()

                file(READ "${SOURCE_DIR}/${dir}.properties" PROPERTIES_CONTENTS)
                string(REPLACE "\r\n" "\n" PROPERTIES_CONTENTS "${PROPERTIES_CONTENTS}")
                string(REPLACE "\r" "\n" PROPERTIES_CONTENTS "${PROPERTIES_CONTENTS}")
                
                string(REGEX MATCH "description[ \t]*=[ \t]*([^\n\r]*)" _match "${PROPERTIES_CONTENTS}")
                set(COMPONENT_DESCRIPTION "${CMAKE_MATCH_1}")
                
                cpack_add_component(${dir} DISPLAY_NAME "${COMPONENT_NAME}" DESCRIPTION "Built-In Game: ${COMPONENT_NAME}\n\n${COMPONENT_DESCRIPTION}" GROUP games)
            endif()
        endif()
    endforeach()

    target_compile_definitions(devgames PRIVATE DEVGAMES_BUILTIN_LIST="${DEVGAMES_BUILTIN_LIST}")
endif()