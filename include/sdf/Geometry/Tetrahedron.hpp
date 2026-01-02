#pragma once
// Ported from Tetrahedron.glsl
// Copyright Morgan McGuire 2020 - MIT License

#include "../common.hpp"

namespace sdf::geometry {

inline float Tetrahedron(const vec3& p_in, float /*time*/, uint32_t /*seed*/) {
    vec3 p = p_in * 0.5f;
    return std::max(
        // Vertical bound
        std::abs(p.y) - 0.5f,
        // Horizontal bound
        std::max(std::abs(p.x) * 0.866025f + p.z * 0.5f, -p.z) - 0.25f * std::abs(0.5f - p.y)
    ) * 2.0f;
}

} // namespace sdf::geometry


