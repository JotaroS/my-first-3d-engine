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

# tinygltf (header only)
FetchContent_Declare(
    tinygltf
    GIT_REPOSITORY https://github.com/syoyo/tinygltf.git
    GIT_TAG        v2.8.9
)

FetchContent_MakeAvailable(tinygltf)

# ImGui source files
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui)
file(GLOB IMGUI_SOURCES 
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# executables (all files in src/ recursively)
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.cpp src/*.h)
add_executable(${PROJECT_NAME} ${SRC_FILES} external/src/glad.c ${IMGUI_SOURCES})

# include directories for headers
target_include_directories(${PROJECT_NAME} PRIVATE 
    external/include/
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

# link libraries with opengl

find_package(OpenGL REQUIRED) # this finds opengl package from your system

target_link_libraries(FirstRender PRIVATE
    glfw
    OpenGL::GL        # Win/Mac両対応
    glm::glm
)


# mac のときは追加でフラグが欲しければここで
# if(APPLE)
