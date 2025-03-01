cmake_minimum_required(VERSION 3.23)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/scripts/cmake/CMakeToolchainDeluge.cmake)

project(Deluge
    LANGUAGES CXX C ASM
    VERSION 1.0.0
)

include(ProcessorCount)
ProcessorCount(HOST_NUM_CORES)

# Check for IPO support
include(CheckIPOSupported)
check_ipo_supported(RESULT IPO_SUPPORTED OUTPUT error)

if(IPO_SUPPORTED)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION
        $<$<CONFIG:DEBUG>:OFF>
        $<$<CONFIG:RELWITHDEBINFO>:ON>
        $<$<CONFIG:RELEASE>:ON>
    )
endif()

# Create our version string
set(RELEASE_TYPE "dev" CACHE STRING "The type of releasepack to build")
set_property(CACHE RELEASE_TYPE PROPERTY STRINGS dev nightly alpha beta rc release)

if(RELEASE_TYPE STREQUAL "release")
    set(SEMVER_PRERELEASE_ID "")
else()
    set(SEMVER_PRERELEASE_ID "-${RELEASE_TYPE}")
endif()

find_package(Git)

if(GIT_FOUND)
    message(VERBOSE "git found: ${GIT_EXECUTABLE} version ${GIT_VERSION_STRING}")
else()
    message(FATAL_ERROR "Git not found. Check your path.")
endif(GIT_FOUND)

# Get the result of "git rev-parse --short HEAD"
execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_SHORT
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get the result of "git status --porcelain"
execute_process(
    COMMAND ${GIT_EXECUTABLE} status --porcelain
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_STATUS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Today's date
string(TIMESTAMP DATE_TODAY "%Y.%m.%d")

# Build our version string
set(BUILD_VERSION_STRING "${PROJECT_VERSION}${SEMVER_PRERELEASE_ID}-${GIT_COMMIT_SHORT}") # Used in firmware
set(BUILD_VERSION_SUFFIX "-v${PROJECT_VERSION}${SEMVER_PRERELEASE_ID}+${DATE_TODAY}-${GIT_COMMIT_SHORT}") # Used for filename

string(REPLACE "." "_" BUILD_VERSION_SUFFIX ${BUILD_VERSION_SUFFIX})

if(NOT "${GIT_STATUS}" STREQUAL "")
    string(APPEND BUILD_VERSION_SUFFIX "-dirty")
    string(APPEND BUILD_VERSION_STRING "-dirty")
endif()

message(STATUS "Deluge version: v${BUILD_VERSION_STRING}")

option(ENABLE_VERSIONED_OUTPUT "Tag the resulting output files with a semver label" OFF)

if(ENABLE_VERSIONED_OUTPUT)
    set(EXECUTABLE_VERSION_SUFFIX ${BUILD_VERSION_SUFFIX})
else()
    set(EXECUTABLE_VERSION_SUFFIX "")
endif(ENABLE_VERSIONED_OUTPUT)

# Global compilation options
add_compile_options(

    # Feature flags
    -fdiagnostics-parseable-fixits
    -fsigned-char

    # Debug symbols
    $<$<CONFIG:DEBUG>:-g> # Include
    $<$<CONFIG:DEBUG>:-ggdb> # Include
    $<$<CONFIG:RELWITHDEBINFO>:-g> # Include
    $<$<CONFIG:RELEASE>:-s> # Strip

    # Optimization level
    $<$<CONFIG:DEBUG>:-Og>
    $<$<CONFIG:RELWITHDEBINFO>:-O2>
    $<$<CONFIG:RELEASE>:-O2>
    -fno-inline-functions
    # Link time optimizations
    $<$<CONFIG:RELWITHDEBINFO>:-flto>
    $<$<CONFIG:RELEASE>:-flto>
    -ffunction-sections
    -fdata-sections

    # C++ stuff
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-use-cxa-atexit>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-threadsafe-statics>

    # ASM stuff
    $<$<COMPILE_LANGUAGE:ASM>:-x>
    $<$<COMPILE_LANGUAGE:ASM>:assembler-with-cpp>
)

add_link_options(
    -T ${PROJECT_SOURCE_DIR}/linker_script_rz_a1l.ld
    LINKER:--gc-sections

    -nostartfiles # Don't emit startfiles
    -estart # Set the entrypoint to 'start' (see RZA1/compiler/asm/start.S)
)

# RTT Debug options
option(ENABLE_RTT "Enable RTT output" ON)
option(ENABLE_SYSEX_LOAD "Enable loading firmware over midi sysex" OFF)

# Colored output
option(FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)." ON)

if(FORCE_COLORED_OUTPUT)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        message(STATUS "Forcing colors for GCC")
        add_compile_options(-fdiagnostics-color=always)
        add_link_options(-fdiagnostics-color=always)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        message(STATUS "Forcing colors for Clang")
        add_compile_options(-fcolor-diagnostics)
    endif()
endif(FORCE_COLORED_OUTPUT)

# ##### Documentation generation #######
# check if Doxygen is installed
find_package(Doxygen
    OPTIONAL_COMPONENTS dot mscgen dia
)

if(DOXYGEN_FOUND)
    if(NOT ${DOXYGEN_HAVE_DOT})
        message(
            "Can't find GraphViz DOT tool for generating images."
            "Make sure it's on your PATH or install GraphViz")
    endif()

    set(DOXYGEN_PROJECT_NAME "Deluge Firmware")
    set(DOXYGEN_RECURSIVE YES)
    set(DOXYGEN_EXAMPLE_RECURSIVE YES)

    set(DOXYGEN_NUM_PROC_THREADS ${HOST_NUM_CORES})

    # From doxygen-awesome
    set(DOXYGEN_GENERATE_TREEVIEW YES)
    set(DOXYGEN_DISABLE_INDEX NO)
    set(DOXYGEN_FULL_SIDEBAR NO)
    set(DOXYGEN_HTML_COLORSTYLE LIGHT)
    set(DOXYGEN_HTML_EXTRA_STYLESHEET "${doxygen-awesome-css_SOURCE_DIR}/doxygen-awesome.css")

    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE README.md)

    file(GLOB markdown_SOURCES *.md)

    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/html/)

    doxygen_add_docs(doxygen ${markdown_SOURCES} docs src/deluge)
else(DOXYGEN_FOUND)
    message(WARNING
        "Doxygen needs to be installed to generate documentation."
        "Please install from https://github.com/doxygen/doxygen/releases")
endif(DOXYGEN_FOUND)

# Add libraries
add_subdirectory(lib)

# Add the main executable target
add_executable(deluge)

# Add our sources
add_subdirectory(src)

# Link external libraries
# target_link_libraries(deluge PUBLIC

# )

# Project-only compile options (warnings, etc.)
target_compile_options(deluge PUBLIC

    # Warnings
    # $<$<CONFIG:DEBUG>:-Wextra> # TODO: Enable when ready
    # $<$<CONFIG:DEBUG>:-Wall> # TODO: Enable when ready
    # $<$<CONFIG:DEBUG>:-Wpedantic> # TODO: Enable when ready
    $<$<CONFIG:DEBUG>:-Wnull-dereference>
    $<$<CONFIG:DEBUG>:-Wno-psabi>

    $<$<CONFIG:DEBUG>:-Werror=write-strings>

    # Offsetof for non standard types is supported in GCC, 
    # if another compiler does not support it they are obligated to error

    # Stack usage
    # tracked for diagnostic usage. We have a 32k stack so it's not that important
    # primarily concerned with unbounded stack use
    $<$<CONFIG:DEBUG>:-Wstack-usage=2000>
    $<$<CONFIG:RELWITHDEBINFO>:-Wstack-usage=2000>
    $<$<CONFIG:RELEASE>:-Wstack-usage=2000>
)

set_target_properties(deluge
    PROPERTIES
        C_STANDARD 11
        C_STANDARD_REQUIRED ON
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS ON
        SUFFIX "${EXECUTABLE_VERSION_SUFFIX}.elf"
)

set(BIN_FILE $<CONFIG>/deluge${EXECUTABLE_VERSION_SUFFIX}.bin)
set(HEX_FILE $<CONFIG>/deluge${EXECUTABLE_VERSION_SUFFIX}.hex)
set(MAP_FILE $<CONFIG>/deluge${EXECUTABLE_VERSION_SUFFIX}.map)

# add link options to generate memory map
target_link_options(deluge PUBLIC
    LINKER:-Map=${MAP_FILE}
    LINKER:--cref
)

# generate .bin firmware file
add_custom_command(TARGET deluge
    POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O binary -S $<TARGET_FILE:deluge> ${BIN_FILE}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Objcopying deluge to firmware ${BIN_FILE}"
        DEPENDS deluge
        BYPRODUCTS ${BIN_FILE})

# generate .hex firmware file
add_custom_command(TARGET deluge
    POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O ihex -S $<TARGET_FILE:deluge> ${HEX_FILE}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Objcopying deluge to firmware ${HEX_FILE}"
        DEPENDS deluge
        BYPRODUCTS ${HEX_FILE})

# Clean up artefacts
set_target_properties(deluge
    PROPERTIES
        ADDITIONAL_CLEAN_FILES ${BIN_FILE} ${MAP_FILE} ${HEX_FILE}
)

# find_program(PRETTYSIZE_CMD prettysize prettysize.py)

# if(NOT PRETTYSIZE_CMD STREQUAL "PRETTYSIZE_CMD-NOTFOUND")
# add_custom_command(
# TARGET deluge POST_BUILD
# COMMAND prettysize
# -l ${CMAKE_CURRENT_LIST_DIR}/linker_script_rz_a1l.ld
# -s ${CMAKE_SIZE} $<TARGET_FILE:deluge>
# WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
# COMMENT "Displaying usage graph for deluge")
# endif()
