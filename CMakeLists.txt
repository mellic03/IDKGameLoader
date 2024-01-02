cmake_minimum_required(VERSION 3.9)
project(IDKGameLoader)


set(CMAKE_BUILD_PARALLEL_LEVEL 8)
set(CMAKE_CXX_FLAGS "-std=c++17 -Wuninitialized -Winit-self -Wmaybe-uninitialized -Werror -fsanitize=undefined -fdiagnostics-color=always -lstdc++fs")
add_compile_options("-g" "-rdynamic" "-fno-ident" "-fno-asynchronous-unwind-tables")
add_definitions(-DIDK_DEBUG)


include_directories(${CMAKE_SOURCE_DIR}/external/include/)

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


target_link_libraries(
    ${PROJECT_NAME} PUBLIC
    GLEW
    SDL2
    SDL2_image
    SDL2_mixer
    ${OPENGL_LIBRARIES}

    libidk
    IDKGameEngine
)
