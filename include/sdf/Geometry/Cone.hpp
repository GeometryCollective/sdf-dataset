#pragma once
// Ported from Cone.glsl
// Copyright 2013 Inigo Quilez @iq - MIT License

#include "../common.hpp"

namespace sdf::geometry {

namespace detail {
    inline float sdCone(const vec3& p, const vec2& c, float h) {
        float q = length(vec2(p.x, p.z));
        return std::max(dot(c, vec2(q, p.y)), -h - p.y);
    }
}

inline float Cone(const vec3& p, float /*time*/, uint32_t /*seed*/) {
    const float a = 1.11f;
    return detail::sdCone(p - vec3(0.0f, 1.0f, 0.0f), vec2(sin(a), cos(a)), 2.0f);
}

} // namespace sdf::geometry


