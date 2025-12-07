# cmake version 

cmake_minimum_required(VERSION 4.1)

#project name

project(FirstRender
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# fetch content libraries

include(FetchContent)

# GLFW
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw)

# GLM (header only)
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        0af55ccecd98d4e5a8d1fad7de25ba429d60e863
)

FetchContent_MakeAvailable(glm)

# executables (all files in src/ recursively)
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.cpp src/*.h)
add_executable(${PROJECT_NAME} ${SRC_FILES} external/src/glad.c)

# include directories for headers
target_include_directories(${PROJECT_NAME} PRIVATE external/include/)

# link libraries with opengl

find_package(OpenGL REQUIRED) # this finds opengl package from your system

target_link_libraries(FirstRender PRIVATE
    glfw
    OpenGL::GL        # Win/Mac両対応
    glm::glm
)


# mac のときは追加でフラグが欲しければここで
if(APPLE)
    # 例えば将来 Metal に移行したくなったら分岐を切る場所
endif()