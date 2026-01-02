#pragma once

// Main API for evaluating signed distance functions
//
// Usage:
//   std::vector<glm::vec3> points = { ... };
//   std::vector<float> distances = sdf::evaluate("Sphere", points);
//
// The SDF name should match the filename (without extension) of the original
// GLSL file, e.g., "Sphere", "Fish", "Mandelbulb", etc.

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <cstdint>

namespace sdf {

/// Evaluate an SDF at multiple points.
///
/// @param name   The name of the SDF (e.g., "Sphere", "Fish", "Mandelbulb")
/// @param points The query points in R^3
/// @param time   Time parameter for animated SDFs (default: 0.0)
/// @param seed   Random seed for procedural SDFs (default: 12345)
/// @return       Vector of signed distances, one per input point
/// @throws       std::runtime_error if the SDF name is unknown
std::vector<float> evaluate(
    const std::string& name,
    const std::vector<glm::vec3>& points,
    float time = 0.0f,
    uint32_t seed = 12345
);

/// Evaluate an SDF at a single point.
///
/// @param name  The name of the SDF (e.g., "Sphere", "Fish", "Mandelbulb")
/// @param point The query point in R^3
/// @param time  Time parameter for animated SDFs (default: 0.0)
/// @param seed  Random seed for procedural SDFs (default: 12345)
/// @return      Signed distance at the query point
/// @throws      std::runtime_error if the SDF name is unknown
float evaluate(
    const std::string& name,
    const glm::vec3& point,
    float time = 0.0f,
    uint32_t seed = 12345
);

/// Get a list of all available SDF names.
///
/// @return Vector of SDF names that can be passed to evaluate()
std::vector<std::string> getAvailableSDFs();

} // namespace sdf


