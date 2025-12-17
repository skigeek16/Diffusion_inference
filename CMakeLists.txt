cmake_minimum_required(VERSION 3.15)
project(StableDiffusionGUI VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Options
option(USE_CUDA "Build with CUDA support" OFF)
option(USE_VULKAN "Build with Vulkan support" OFF)

include(FetchContent)

# Fetch GLFW
message(STATUS "Fetching GLFW...")
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# Fetch Dear ImGui
message(STATUS "Fetching Dear ImGui...")
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.5
)
FetchContent_MakeAvailable(imgui)

# Fetch stb
message(STATUS "Fetching stb...")
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
)
FetchContent_MakeAvailable(stb)

# Create GLAD library (using local files)
add_library(glad STATIC
    src/glad/src/glad.c
)
target_include_directories(glad PUBLIC src/glad/include)

# Create ImGui library
add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC 
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(imgui PUBLIC glfw glad)

# stable-diffusion.cpp integration (assuming it's in a subdirectory)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/stable-diffusion.cpp/CMakeLists.txt")
    message(STATUS "Found stable-diffusion.cpp")
    add_subdirectory(stable-diffusion.cpp)
else()
    message(WARNING "stable-diffusion.cpp not found. Clone it to the project root:")
    message(WARNING "  git clone --recursive https://github.com/leejet/stable-diffusion.cpp")
endif()

# GUI Application
add_executable(sd_gui
    src/main.cpp
    src/gui_app.cpp
    src/sd_generator.cpp
    src/image_viewer.cpp
)

target_include_directories(sd_gui PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${stb_SOURCE_DIR}
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${CMAKE_CURRENT_SOURCE_DIR}/stable-diffusion.cpp
)

target_link_libraries(sd_gui PRIVATE
    imgui
    glfw
    glad
)

# Link to stable-diffusion.cpp if available
if(TARGET stable-diffusion)
    target_link_libraries(sd_gui PRIVATE stable-diffusion)
endif()

# Windows-specific settings
if(WIN32)
    target_compile_definitions(sd_gui PRIVATE
        _CRT_SECURE_NO_WARNINGS
        NOMINMAX
    )
    # Add console for debugging (remove for release)
    # set_target_properties(sd_gui PROPERTIES WIN32_EXECUTABLE FALSE)
endif()

# Print configuration
message(STATUS "Stable Diffusion GUI Configuration:")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  CUDA Support: ${USE_CUDA}")
message(STATUS "  Vulkan Support: ${USE_VULKAN}")

