#pragma once
// Ported from Knob.glsl
// Copyright 2020 Towaki Takikawa @yongyuanxi - MIT License

#include "../common.hpp"

namespace sdf {
namespace manufactured {

namespace knob_detail {
    inline float sdSphere(const vec3& v, float r) {
        return length(v) - r;
    }
    
    inline float sdTorus(const vec3& p, const vec2& t) {
        vec2 q = vec2(length(vec2(p.x, p.z)) - t.x, p.y);
        return length(q) - t.y;
    }
    
    inline float sdCone(const vec3& p, const vec2& c) {
        float q = length(vec2(p.x, p.y));
        return dot(c, vec2(q, p.z));
    }
    
    inline float sdCappedCylinder(const vec3& p, float h, float r) {
        vec2 d = abs(vec2(length(vec2(p.x, p.z)), p.y)) - vec2(h, r);
        return std::min(std::max(d.x, d.y), 0.0f) + length(max(d, vec2(0.0f)));
    }
    
    inline float sdTriPrism(const vec3& p, const vec2& h) {
        vec3 q = abs(p);
        return std::max(q.z - h.y, std::max(q.x * 0.866025f + p.y * 0.5f, -p.y) - h.x * 0.5f);
    }
    
    inline float opSmoothUnion(float d1, float d2, float k) {
        float h = clamp(0.5f + 0.5f * (d2 - d1) / k, 0.0f, 1.0f);
        return mix(d2, d1, h) - k * h * (1.0f - h);
    }
    
    inline float ssub(float d1, float d2, float k) {
        float h = clamp(0.5f - 0.5f * (d2 + d1) / k, 0.0f, 1.0f);
        return mix(d2, -d1, h) + k * h * (1.0f - h);
    }
    
    inline float sdBase(vec3 p) {
        float base = opSmoothUnion(
            sdCone(rotationMatrix(vec3(1.0f, 0.0f, 0.0f), -pi / 2.0f) * (p + vec3(0.0f, 0.9f, 0.0f)), vec2(pi / 3.0f)),
            sdCone(rotationMatrix(vec3(1.0f, 0.0f, 0.0f), pi / 2.0f) * (p - vec3(0.0f, 0.9f, 0.0f)), vec2(pi / 3.0f)),
            0.02f);
        base = std::max(base, sdCappedCylinder(p, 1.1f, 0.25f)) * 0.7f;
        base = std::max(-sdCappedCylinder(p, 0.6f, 0.3f), base);
        base = std::max(-sdTriPrism(rotationMatrix(vec3(1.0f, 0.0f, 0.0f), pi / 2.0f) * (p + vec3(0.0f, 0.0f, -1.0f)), vec2(1.2f, 0.3f)), base);
        return base;
    }
    
    inline float sdKnob(const vec3& p) {
        float sphere = sdSphere(p, 1.0f);
        float cutout = sdSphere(p - vec3(0.0f, 0.5f, 0.5f), 0.7f);
        float cutout_etch = sdTorus(rotationMatrix(vec3(1.0f, 0.0f, 0.0f), -pi / 4.0f) * (p - vec3(0.0f, 0.2f, 0.2f)), vec2(1.0f, 0.05f));
        float innersphere = sdSphere(p - vec3(0.0f, 0.0f, 0.0f), 0.75f);
        
        float d = ssub(cutout, sphere, 0.1f);
        d = std::min(d, innersphere);
        d = std::max(-cutout_etch, d);
        d = std::min(ssub(sphere, sdBase(p - vec3(0.0f, -0.775f, 0.0f)), 0.1f), d);
        return d;
    }
}

inline float Knob(const vec3& p, float /*time*/, uint32_t /*seed*/) {
    const float scale = 0.8f;
    return knob_detail::sdKnob(p * (1.0f / scale)) * scale;
}

} // namespace manufactured
} // namespace sdf

