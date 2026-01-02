#include "sdf/sdf.hpp"

#include <stdexcept>
#include <unordered_map>
#include <algorithm>

// Include all SDF headers
// Geometry
#include "sdf/Geometry/Sphere.hpp"
#include "sdf/Geometry/Cube.hpp"
#include "sdf/Geometry/Torus.hpp"
#include "sdf/Geometry/Capsule.hpp"
#include "sdf/Geometry/Cylinder.hpp"
#include "sdf/Geometry/Cone.hpp"
#include "sdf/Geometry/Roundbox.hpp"
#include "sdf/Geometry/Hexprism.hpp"
#include "sdf/Geometry/Octahedron.hpp"
#include "sdf/Geometry/Octabound.hpp"
#include "sdf/Geometry/Pyramid.hpp"
#include "sdf/Geometry/Tetrahedron.hpp"
#include "sdf/Geometry/Icosahedron.hpp"
#include "sdf/Geometry/Dodecahedron.hpp"
#include "sdf/Geometry/Triprismbound.hpp"
#include "sdf/Geometry/Triangle.hpp"
#include "sdf/Geometry/Bezier.hpp"
#include "sdf/Geometry/Trefoil.hpp"
#include "sdf/Geometry/Helix.hpp"

// Fractal
#include "sdf/Fractal/Mandelbulb.hpp"
#include "sdf/Fractal/Menger.hpp"
#include "sdf/Fractal/Serpinski.hpp"
#include "sdf/Fractal/Julia.hpp"

// Animal
#include "sdf/Animal/Fish.hpp"

// Manufactured
#include "sdf/Manufactured/Teapot.hpp"
#include "sdf/Manufactured/Gear.hpp"

// Misc
#include "sdf/Misc/Burger.hpp"

namespace sdf {

// Type alias for SDF function pointer
using SDFFunc = float(*)(const glm::vec3&, float, uint32_t);

// Registry of all available SDFs
static const std::unordered_map<std::string, SDFFunc> g_registry = {
    // Geometry
    {"Sphere", geometry::Sphere},
    {"Cube", geometry::Cube},
    {"Torus", geometry::Torus},
    {"Capsule", geometry::Capsule},
    {"Cylinder", geometry::Cylinder},
    {"Cone", geometry::Cone},
    {"Roundbox", geometry::Roundbox},
    {"Hexprism", geometry::Hexprism},
    {"Octahedron", geometry::Octahedron},
    {"Octabound", geometry::Octabound},
    {"Pyramid", geometry::Pyramid},
    {"Tetrahedron", geometry::Tetrahedron},
    {"Icosahedron", geometry::Icosahedron},
    {"Dodecahedron", geometry::Dodecahedron},
    {"Triprismbound", geometry::Triprismbound},
    {"Triangle", geometry::Triangle},
    {"Bezier", geometry::Bezier},
    {"Trefoil", geometry::Trefoil},
    {"Helix", geometry::Helix},
    
    // Fractal
    {"Mandelbulb", fractal::Mandelbulb},
    {"Menger", fractal::Menger},
    {"Serpinski", fractal::Serpinski},
    {"Julia", fractal::Julia},
    
    // Animal
    {"Fish", animal::Fish},
    
    // Manufactured
    {"Teapot", manufactured::Teapot},
    {"Gear", manufactured::Gear},
    
    // Misc
    {"Burger", misc::Burger},
};

std::vector<float> evaluate(
    const std::string& name,
    const std::vector<glm::vec3>& points,
    float time,
    uint32_t seed
) {
    auto it = g_registry.find(name);
    if (it == g_registry.end()) {
        throw std::runtime_error("Unknown SDF: " + name);
    }
    
    SDFFunc func = it->second;
    std::vector<float> results;
    results.reserve(points.size());
    
    for (const auto& p : points) {
        results.push_back(func(p, time, seed));
    }
    
    return results;
}

float evaluate(
    const std::string& name,
    const glm::vec3& point,
    float time,
    uint32_t seed
) {
    auto it = g_registry.find(name);
    if (it == g_registry.end()) {
        throw std::runtime_error("Unknown SDF: " + name);
    }
    
    return it->second(point, time, seed);
}

std::vector<std::string> getAvailableSDFs() {
    std::vector<std::string> names;
    names.reserve(g_registry.size());
    
    for (const auto& [name, func] : g_registry) {
        names.push_back(name);
    }
    
    std::sort(names.begin(), names.end());
    return names;
}

} // namespace sdf


