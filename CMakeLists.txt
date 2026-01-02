cmake_minimum_required(VERSION 3.16)
project(conservative_sdf VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================================================
# Dependencies
# ============================================================================

add_subdirectory(deps/polyscope)

# ============================================================================
# SDF Library
# ============================================================================

add_library(sdf_lib STATIC
    src/sdf.cpp
)

target_include_directories(sdf_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${polyscope_SOURCE_DIR}/deps/glm  # Use Polyscope's glm
)

# Enable GLM experimental features for swizzling
target_compile_definitions(sdf_lib PUBLIC
    GLM_ENABLE_EXPERIMENTAL
    GLM_FORCE_SWIZZLE
)

# ============================================================================
# Main Executable (Polyscope visualization)
# ============================================================================

add_executable(sdf_viewer
    src/main.cpp
)

target_link_libraries(sdf_viewer PRIVATE
    sdf_lib
    polyscope
)
