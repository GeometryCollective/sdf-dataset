#pragma once
// Ported from Triangle.glsl
// Copyright 2013 Inigo Quilez @iq - MIT License

#include "../common.hpp"

namespace sdf::geometry {

namespace triangle_detail {
    inline float dot2(const vec3& v) { return dot(v, v); }
    
    // Unsigned distance to triangle (no inside/outside since it's a flat shape)
    inline float udTriangle(const vec3& p, const vec3& a, const vec3& b, const vec3& c) {
        vec3 ba = b - a; vec3 pa = p - a;
        vec3 cb = c - b; vec3 pb = p - b;
        vec3 ac = a - c; vec3 pc = p - c;
        vec3 nor = cross(ba, ac);
        
        return std::sqrt(
            (sign(dot(cross(ba, nor), pa)) +
             sign(dot(cross(cb, nor), pb)) +
             sign(dot(cross(ac, nor), pc)) < 2.0f)
            ?
            std::min(std::min(
                dot2(ba * clamp(dot(ba, pa) / dot2(ba), 0.0f, 1.0f) - pa),
                dot2(cb * clamp(dot(cb, pb) / dot2(cb), 0.0f, 1.0f) - pb)),
                dot2(ac * clamp(dot(ac, pc) / dot2(ac), 0.0f, 1.0f) - pc))
            :
            dot(nor, pa) * dot(nor, pa) / dot2(nor));
    }
} // namespace triangle_detail

inline float Triangle(const vec3& p, float /*time*/, uint32_t /*seed*/) {
    return triangle_detail::udTriangle(p, vec3(-1.0f, -1.0f, 0.0f), vec3(1.0f, -1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
}

} // namespace sdf::geometry


