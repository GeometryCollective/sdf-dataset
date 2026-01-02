#pragma once
// Ported from Cylinder.glsl
// Copyright 2013 Inigo Quilez @iq - MIT License

#include "../common.hpp"

namespace sdf::geometry {

namespace detail {
    inline float sdCappedCylinder(const vec3& p, float h, float r) {
        vec2 d = abs(vec2(length(vec2(p.x, p.z)), p.y)) - vec2(h, r);
        return std::min(std::max(d.x, d.y), 0.0f) + length(max(d, vec2(0.0f)));
    }
}

inline float Cylinder(const vec3& p, float /*time*/, uint32_t /*seed*/) {
    return detail::sdCappedCylinder(p, 1.0f, 1.0f);
}

} // namespace sdf::geometry


