#pragma once
// Ported from Dodecahedron.glsl
// Copyright 2016 Thomas Hooper @tdhooper - CC BY-NC-SA 3.0

#include "../common.hpp"

namespace sdf::geometry {

namespace dodecahedron_detail {
    constexpr float PHI = 1.618033988749895f;
    
    inline float fDodecahedron(const vec3& p, float r) {
        // GDF vectors for dodecahedron
        const vec3 v13 = normalize(vec3(0, PHI, 1));
        const vec3 v14 = normalize(vec3(0, -PHI, 1));
        const vec3 v15 = normalize(vec3(1, 0, PHI));
        const vec3 v16 = normalize(vec3(-1, 0, PHI));
        const vec3 v17 = normalize(vec3(PHI, 1, 0));
        const vec3 v18 = normalize(vec3(-PHI, 1, 0));
        
        float d = 0.0f;
        d = std::max(d, std::abs(dot(p, v13)));
        d = std::max(d, std::abs(dot(p, v14)));
        d = std::max(d, std::abs(dot(p, v15)));
        d = std::max(d, std::abs(dot(p, v16)));
        d = std::max(d, std::abs(dot(p, v17)));
        d = std::max(d, std::abs(dot(p, v18)));
        return d - r;
    }
} // namespace dodecahedron_detail

inline float Dodecahedron(const vec3& p, float /*time*/, uint32_t /*seed*/) {
    const float scale = 0.7f;
    return dodecahedron_detail::fDodecahedron(p * (1.0f / scale), 1.0f) * scale;
}

} // namespace sdf::geometry


