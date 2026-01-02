#pragma once
// Ported from Octahedron.glsl
// Copyright 2013 Inigo Quilez @iq - MIT License

#include "../common.hpp"

namespace sdf::geometry {

namespace detail {
    inline float sdOctahedron(vec3 p, float s) {
        p = abs(p);
        float m = p.x + p.y + p.z - s;
        vec3 q;
        if (3.0f * p.x < m) q = p;
        else if (3.0f * p.y < m) q = vec3(p.y, p.z, p.x);
        else if (3.0f * p.z < m) q = vec3(p.z, p.x, p.y);
        else return m * 0.57735027f;
        
        float k = clamp(0.5f * (q.z - q.y + s), 0.0f, s);
        return length(vec3(q.x, q.y - s + k, q.z - k));
    }
}

inline float Octahedron(const vec3& p, float /*time*/, uint32_t /*seed*/) {
    return detail::sdOctahedron(p, 0.5f);
}

} // namespace sdf::geometry


