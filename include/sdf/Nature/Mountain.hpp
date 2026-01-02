#pragma once
// Ported from Mountain.glsl
// Copyright 2020 Morgan McGuire @CasualEffects - MIT License

#include "../common.hpp"

namespace sdf {
namespace nature {

namespace mountain_detail {
    inline float sdBox(const vec3& p, const vec3& b) {
        vec3 d = abs(p) - b;
        return std::min(std::max(d.x, std::max(d.y, d.z)), 0.0f) + length(max(d, vec3(0.0f)));
    }
    
    inline float mountain_hash(const vec2& p) {
        return fract(1e4f * std::sin(17.0f * p.x + p.y * 0.1f) * (0.1f + std::abs(std::sin(p.y * 13.0f + p.x))));
    }
    
    inline float mountain_noise1(const vec2& x) {
        vec2 i = floor(x);
        vec2 f = fract(x);
        
        float a = mountain_hash(i);
        float b = mountain_hash(i + vec2(1.0f, 0.0f));
        float c = mountain_hash(i + vec2(0.0f, 1.0f));
        float d = mountain_hash(i + vec2(1.0f, 1.0f));
        
        vec2 u = f * f * (vec2(3.0f) - 2.0f * f);
        return mix(a, b, u.x) + (c - a) * u.y * (1.0f - u.x) + (d - b) * u.x * u.y;
    }
    
    inline float mountain_noise(vec2 x, int numOctaves) {
        float v = 0.0f;
        float a = 0.5f;
        vec2 shift = vec2(100.0f, 50.0f);
        float c05 = std::cos(0.5f);
        float s05 = std::sin(0.5f);
        mat2 rot = mat2(c05, s05, -s05, c05);
        for (int i = 0; i < numOctaves; ++i) {
            v += a * mountain_noise1(x);
            x = rot * x * 2.0f + shift;
            a *= 0.5f;
        }
        return v;
    }
}

inline float Mountain(const vec3& p, float /*time*/, uint32_t /*seed*/) {
    // Measure distance from a conservative bounding box for speed when far away
    float dBigBox = mountain_detail::sdBox(p, vec3(0.75f));
    if (dBigBox > 0.25f) {
        return dBigBox;
    }
    
    float dBox = mountain_detail::sdBox(p, vec3(0.5f));
    
    vec2 offset = vec2(p.x, p.z) - vec2(0.05f, 0.07f);
    float d = dot(offset, offset);
    d = 0.8f * std::pow(d, 0.36987f + 0.00415f / d);
    float terrainVertical = (p.y + d - mountain_detail::mountain_noise(vec2(p.x, p.z) * 2.0f + vec2(0.6f, 1.6f), 9) + 0.3f);
    
    float lakeVertical = p.y + 0.48f;
    
    const float conservativeFactor = 0.3f;
    float terrain = std::min(terrainVertical, lakeVertical) * conservativeFactor;
    return std::max(dBox, terrain);
}

} // namespace nature
} // namespace sdf

