#pragma once
// Ported from Cube.glsl
// Copyright 2013 Inigo Quilez @iq - MIT License

#include "../common.hpp"

namespace sdf::geometry {

namespace cube_detail {
    inline float sdBox(const vec3& p, const vec3& b) {
        vec3 d = abs(p) - b;
        return std::min(std::max(d.x, std::max(d.y, d.z)), 0.0f) + length(max(d, vec3(0.0f)));
    }
} // namespace cube_detail

inline float Cube(const vec3& p, float /*time*/, uint32_t /*seed*/) {
    return cube_detail::sdBox(p, vec3(0.5f));
}

} // namespace sdf::geometry


