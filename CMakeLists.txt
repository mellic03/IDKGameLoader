cmake_minimum_required(VERSION 3.9)
project(IDKGameLoader)

file(GLOB SOURCES
    "src/*.cpp"
)


# Non-IDK dependencies
# ---------------------------------------------------
set(OpenGL_GL_PREFERENCE GLVND)

find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)

include_directories(${GLEW_INCLUDE_DIRS})
include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${SDL2_image_INCLUDE_DIRS})
include_directories(${SDL2_mixer_INCLUDE_DIRS})


# ---------------------------------------------------


add_executable(${PROJECT_NAME} ${SOURCES})


target_include_directories(
    ${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../IDKGameEngine/thirdparty/physx/include/
)

target_link_libraries(
    ${PROJECT_NAME} PUBLIC
    GLEW
    SDL2
    SDL2_image
    SDL2_mixer
    ${OPENGL_LIBRARIES}

    IDKGraphics
    IDKGameEngine
)


file(
    COPY ${CMAKE_CURRENT_SOURCE_DIR}/IDKGE/
    DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/IDKGE/
)