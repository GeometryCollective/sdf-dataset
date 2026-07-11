cmake_minimum_required(VERSION 3.16)
project(conservative_sdf VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build the Polyscope-based sdf_viewer demo? Turn OFF for a headless / library-only
# build with no OpenGL/GLFW dependency. A parent project can force this OFF.
option(SDF_WITH_VIEWER "Build the Polyscope-based sdf_viewer executable" ON)

# ============================================================================
# Dependencies
# ============================================================================

# Add Polyscope when the viewer is enabled (unless a parent already added it).
# Doing this first means the default build reuses Polyscope's bundled glm below,
# exactly as before -- the standalone-glm path only kicks in for headless builds.
if (SDF_WITH_VIEWER AND NOT TARGET polyscope)
    add_subdirectory(deps/polyscope)
endif()

# glm is part of sdf_lib's public interface (the SDF headers expose glm types).
# Reuse the copy Polyscope bundles when it is available; otherwise fetch a
# standalone copy so sdf_lib can be built with no Polyscope dependency at all.
if (TARGET polyscope)
    set(SDF_GLM_INCLUDE_DIR ${polyscope_SOURCE_DIR}/deps/glm)  # Use Polyscope's glm
else()
    include(FetchContent)
    FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.1
    )
    FetchContent_MakeAvailable(glm)
    set(SDF_GLM_INCLUDE_DIR ${glm_SOURCE_DIR})
endif()

# ============================================================================
# SDF Library
# ============================================================================

add_library(sdf_lib STATIC
    src/sdf.cpp
)

target_include_directories(sdf_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${SDF_GLM_INCLUDE_DIR}
)

# Enable GLM experimental features for swizzling
target_compile_definitions(sdf_lib PUBLIC
    GLM_ENABLE_EXPERIMENTAL
    GLM_FORCE_SWIZZLE
)

# Create namespace alias
add_library(sdf_dataset::sdf_lib ALIAS sdf_lib)

# ============================================================================
# Viewer executable (optional, Polyscope visualization)
# ============================================================================

if (SDF_WITH_VIEWER)
    add_executable(sdf_viewer
        src/main.cpp
    )

    target_link_libraries(sdf_viewer PRIVATE
        sdf_lib
        polyscope
    )
endif()
