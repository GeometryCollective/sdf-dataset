#pragma once
// Ported from Vase.glsl
// Copyright 2018 Wes Bakane @WB - CC BY-NC-SA 3.0

#include "../common.hpp"

namespace sdf {
namespace manufactured {

namespace vase_detail {
    inline float sdSphere(const vec3& pos, const vec3& center, float radius) {
        return length(center - pos) - radius;
    }
    
    inline float sdCappedCylinder(const vec3& p, const vec2& h) {
        vec2 d = abs(vec2(length(vec2(p.x, p.z)), p.y)) - h;
        return std::min(std::max(d.x, d.y), 0.0f) + length(max(d, vec2(0.0f)));
    }
    
    inline float opSmoothUnion(float d1, float d2, float k) {
        float h = clamp(0.5f + 0.5f * (d2 - d1) / k, 0.0f, 1.0f);
        return mix(d2, d1, h) - k * h * (1.0f - h);
    }
    
    inline float opShell(float d, float thickness) { return std::abs(d) - thickness; }
    inline float opSubtraction(float d1, float d2) { return std::max(-d1, d2); }
    
    inline float map(const vec3& pos) {
        float d1 = sdCappedCylinder(pos, vec2(0.2f, 0.75f));
        float d2 = sdSphere(pos, vec3(0.0f, -0.25f, 0.0f), 0.45f);
        float d3 = sdSphere(pos, vec3(0.0f, 1.0f, 0.0f), 0.4f);
        float d4 = sdCappedCylinder(pos + vec3(0.0f, -1.75f, 0.0f), vec2(1.0f, 1.0f));
        
        float df = opSmoothUnion(d1, d2, 0.2f);
        df = opSmoothUnion(df, d3, 0.2f);
        df = opShell(df, 0.01f);
        df = opSubtraction(d4, df);
        df += std::cos(pos.y * 100.0f) / 300.0f;
        
        return df;
    }
}

inline float Vase(const vec3& p, float /*time*/, uint32_t /*seed*/) {
    const float scale = 1.0f;
    return vase_detail::map(p * (1.0f / scale)) * scale;
}

} // namespace manufactured
} // namespace sdf

