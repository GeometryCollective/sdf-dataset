#pragma once
// Ported from Pyramid.glsl
// Copyright 2013 Inigo Quilez @iq - MIT License

#include "../common.hpp"

namespace sdf::geometry {

namespace detail {
    inline float sdPyramid(vec3 p, float h) {
        float m2 = h * h + 0.25f;
        
        p.x = std::abs(p.x);
        p.z = std::abs(p.z);
        if (p.z > p.x) std::swap(p.x, p.z);
        p.x -= 0.5f;
        p.z -= 0.5f;
        
        vec3 q = vec3(p.z, h * p.y - 0.5f * p.x, h * p.x + 0.5f * p.y);
        
        float s = std::max(-q.x, 0.0f);
        float t = clamp((q.y - 0.5f * p.z) / (m2 + 0.25f), 0.0f, 1.0f);
        
        float a = m2 * (q.x + s) * (q.x + s) + q.y * q.y;
        float b = m2 * (q.x + 0.5f * t) * (q.x + 0.5f * t) + (q.y - m2 * t) * (q.y - m2 * t);
        
        float d2 = std::min(q.y, -q.x * m2 - q.y * 0.5f) > 0.0f ? 0.0f : std::min(a, b);
        
        return std::sqrt((d2 + q.z * q.z) / m2) * sign(std::max(q.z, -p.y));
    }
}

inline float Pyramid(const vec3& p, float /*time*/, uint32_t /*seed*/) {
    return detail::sdPyramid((p + vec3(0.0f, 1.0f, 0.0f)) * 0.5f, 1.0f) * 2.0f;
}

} // namespace sdf::geometry


