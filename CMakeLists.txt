cmake_minimum_required(VERSION 3.28)
project(blocks)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 2.6 COMPONENTS system window graphics network audio REQUIRED)
find_package(box2d REQUIRED)

set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/PhysicsWorld.cpp
    src/Block.cpp
    src/Crane.cpp
    src/Tower.cpp
    src/Camera.cpp
)

add_executable(blocks ${SOURCES})

target_include_directories(blocks PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(blocks PRIVATE sfml-graphics box2d::box2d)

# Copy textures and fonts to the output directory
add_custom_command(
    TARGET blocks POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:blocks>/textures"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/textures"             
        "$<TARGET_FILE_DIR:blocks>/textures" 
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:blocks>/fonts"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/fonts"             
        "$<TARGET_FILE_DIR:blocks>/fonts" 
    COMMENT "Syncing assets to binary directory"
)
