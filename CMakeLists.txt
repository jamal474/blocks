cmake_minimum_required(VERSION 3.28)
project(blocks)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 2.6 COMPONENTS system window graphics network audio REQUIRED)
find_package(box2d REQUIRED)
find_package(Python3 REQUIRED COMPONENTS Interpreter)

set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/PhysicsWorld.cpp
    src/Block.cpp
    src/Crane.cpp
    src/Tower.cpp
    src/Camera.cpp
    src/BlockLogger.cpp
)

add_executable(blocks ${SOURCES})

target_include_directories(blocks PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(blocks PRIVATE sfml-graphics box2d::box2d)

# ── Embed binary assets into the executable ──────────────────────────────────
# Each asset is converted to a C++ source file containing a const byte array.
# A single EmbeddedAssets.h header is generated with extern declarations.

set(EMBEDDED_ASSETS_DIR "${CMAKE_BINARY_DIR}/generated")
file(MAKE_DIRECTORY "${EMBEDDED_ASSETS_DIR}")

# Format: "relative/path:VARIABLE_NAME"
set(ASSETS
    "textures/block.png:ASSET_BLOCK_PNG"
    "textures/hook.png:ASSET_HOOK_PNG"
    "textures/background.png:ASSET_BACKGROUND_PNG"
    "fonts/DoodleJump.ttf:ASSET_DOODLEJUMP_TTF"
)

set(GENERATED_SOURCES "")
set(EXTERN_DECLS "")

foreach(ASSET_ENTRY ${ASSETS})
    string(REPLACE ":" ";" ASSET_PARTS "${ASSET_ENTRY}")
    list(GET ASSET_PARTS 0 ASSET_FILE)
    list(GET ASSET_PARTS 1 ASSET_VAR)

    string(TOLOWER "${ASSET_VAR}" ASSET_LOWER)
    set(OUTPUT_CPP "${EMBEDDED_ASSETS_DIR}/${ASSET_LOWER}.cpp")

    message(STATUS "Embedding ${ASSET_FILE} as ${ASSET_VAR} ...")
    execute_process(
        COMMAND "${Python3_EXECUTABLE}"
                "${CMAKE_SOURCE_DIR}/cmake/bin2c.py"
                "${CMAKE_SOURCE_DIR}/${ASSET_FILE}"
                "${ASSET_VAR}"
                "${OUTPUT_CPP}"
        RESULT_VARIABLE BIN2C_RESULT
    )
    if(NOT BIN2C_RESULT EQUAL 0)
        message(FATAL_ERROR "Failed to embed asset: ${ASSET_FILE}")
    endif()

    list(APPEND GENERATED_SOURCES "${OUTPUT_CPP}")
    string(APPEND EXTERN_DECLS
        "extern const unsigned char ${ASSET_VAR}[];\n"
        "extern const std::size_t   ${ASSET_VAR}_SIZE;\n\n")
endforeach()

# Generate the unified header
file(WRITE "${EMBEDDED_ASSETS_DIR}/EmbeddedAssets.h"
    "#pragma once\n"
    "#include <cstddef>\n\n"
    "// Auto-generated asset declarations — DO NOT EDIT\n\n"
    "${EXTERN_DECLS}")

target_sources(blocks PRIVATE ${GENERATED_SOURCES})
target_include_directories(blocks PRIVATE "${EMBEDDED_ASSETS_DIR}")

# ── Windows application icon (.ico compiled into .exe via .rc) ───────────────
if(WIN32)
    execute_process(
        COMMAND "${Python3_EXECUTABLE}"
                "${CMAKE_SOURCE_DIR}/cmake/gen_ico.py"
                "${CMAKE_SOURCE_DIR}/textures/background.png"
                "${CMAKE_BINARY_DIR}/blocks.ico"
        RESULT_VARIABLE ICO_RESULT
    )
    if(ICO_RESULT EQUAL 0)
        file(WRITE "${CMAKE_BINARY_DIR}/blocks.rc"
            "IDI_ICON1 ICON \"blocks.ico\"\n")
        target_sources(blocks PRIVATE "${CMAKE_BINARY_DIR}/blocks.rc")
        message(STATUS "Windows application icon generated successfully")
    else()
        message(WARNING "Could not generate Windows icon from background.png")
    endif()
endif()

if(APPLE)
    add_custom_command(
        TARGET blocks POST_BUILD
        COMMAND codesign --force --deep -s - "$<TARGET_FILE:blocks>"
        COMMENT "Ad-hoc code signing the executable for Gatekeeper"
    )
endif()


# -- Optional test binaries ---------------------------------------------------
# Headless checks for the pendulum maths and a cost-vs-height benchmark.
#   cmake --preset conan-release -DBLOCKS_BUILD_TESTS=ON
option(BLOCKS_BUILD_TESTS "Build the headless simulation tests" OFF)

if(BLOCKS_BUILD_TESTS)
    set(TEST_SUPPORT
        src/Crane.cpp src/Tower.cpp src/Block.cpp
        src/PhysicsWorld.cpp src/BlockLogger.cpp)

    foreach(TEST_NAME test_sim test_perf)
        add_executable(${TEST_NAME} tests/${TEST_NAME}.cpp ${TEST_SUPPORT})
        target_include_directories(${TEST_NAME}
            PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include "${EMBEDDED_ASSETS_DIR}")
        target_link_libraries(${TEST_NAME} PRIVATE sfml-graphics box2d::box2d)
    endforeach()

    enable_testing()
    add_test(NAME simulation COMMAND test_sim)
endif()
