cmake_minimum_required(VERSION 3.9)
project(IDKGameLoader)

file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)


# Non-IDK dependencies
# ---------------------------------------------------
set(OpenGL_GL_PREFERENCE GLVND)

find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf REQUIRED)

include_directories(${GLEW_INCLUDE_DIRS})
include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${SDL2_image_INCLUDE_DIRS})
include_directories(${SDL2_mixer_INCLUDE_DIRS})
include_directories(${SDL2_ttf_INCLUDE_DIRS})
# ---------------------------------------------------


add_executable(${PROJECT_NAME} ${SOURCES})

if (IDK_DEBUG)
    target_compile_options(${PROJECT_NAME} PUBLIC -O0 -g)
else()
    target_compile_options(${PROJECT_NAME} PUBLIC -Ofast)
endif ()


target_include_directories(
    ${PROJECT_NAME} PUBLIC
)

target_link_libraries(
    ${PROJECT_NAME} PUBLIC
    GLEW
    SDL2
    SDL2_image
    SDL2_mixer
    SDL2_ttf
    ${OPENGL_LIBRARIES}

    libidk
    IDKGraphics
    IDKGameEngine
)


file(
    COPY ${CMAKE_CURRENT_SOURCE_DIR}/IDKGE/
    DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/IDKGE/
)