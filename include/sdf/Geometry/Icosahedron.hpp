#pragma once
// Ported from Icosahedron.glsl
// Copyright 2016 Thomas Hooper @tdhooper - CC BY-NC-SA 3.0

#include "../common.hpp"

namespace sdf::geometry {

namespace icosahedron_detail {
    constexpr float PHI = 1.618033988749895f;
    
    inline float fIcosahedron(const vec3& p, float r) {
        // GDF vectors for icosahedron
        const vec3 v3 = normalize(vec3(1, 1, 1));
        const vec3 v4 = normalize(vec3(-1, 1, 1));
        const vec3 v5 = normalize(vec3(1, -1, 1));
        const vec3 v6 = normalize(vec3(1, 1, -1));
        const vec3 v7 = normalize(vec3(0, 1, PHI + 1.0f));
        const vec3 v8 = normalize(vec3(0, -1, PHI + 1.0f));
        const vec3 v9 = normalize(vec3(PHI + 1.0f, 0, 1));
        const vec3 v10 = normalize(vec3(-PHI - 1.0f, 0, 1));
        const vec3 v11 = normalize(vec3(1, PHI + 1.0f, 0));
        const vec3 v12 = normalize(vec3(-1, PHI + 1.0f, 0));
        
        float d = 0.0f;
        d = std::max(d, std::abs(dot(p, v3)));
        d = std::max(d, std::abs(dot(p, v4)));
        d = std::max(d, std::abs(dot(p, v5)));
        d = std::max(d, std::abs(dot(p, v6)));
        d = std::max(d, std::abs(dot(p, v7)));
        d = std::max(d, std::abs(dot(p, v8)));
        d = std::max(d, std::abs(dot(p, v9)));
        d = std::max(d, std::abs(dot(p, v10)));
        d = std::max(d, std::abs(dot(p, v11)));
        d = std::max(d, std::abs(dot(p, v12)));
        return d - r;
    }
} // namespace icosahedron_detail

inline float Icosahedron(const vec3& p, float /*time*/, uint32_t /*seed*/) {
    const float scale = 0.7f;
    return icosahedron_detail::fIcosahedron(p * (1.0f / scale), 1.0f) * scale;
}

} // namespace sdf::geometry


