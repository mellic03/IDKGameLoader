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

set_target_properties(
    ${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
)

# if(MSVC)
#     target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
# else()
#     target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
# endif()


target_link_libraries(
    ${PROJECT_NAME} PUBLIC
    GLEW
    SDL2
    SDL2_image
    SDL2_mixer
    ${OPENGL_LIBRARIES}

    libidk
    IDKGraphics
    IDKGameEngine

    IDKBuiltin
)
