#pragma once
// Ported from Octabound.glsl
// Copyright 2013 Inigo Quilez @iq - MIT License

#include "../common.hpp"

namespace sdf::geometry {

namespace detail {
    inline float sdOctahedronBound(vec3 p, float s) {
        p = abs(p);
        return (p.x + p.y + p.z - s) * 0.57735027f;
    }
}

inline float Octabound(const vec3& p, float /*time*/, uint32_t /*seed*/) {
    return detail::sdOctahedronBound(p, 0.5f);
}

} // namespace sdf::geometry


