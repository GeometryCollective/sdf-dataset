#pragma once
// Ported from Teapot.glsl
// Copyright 2020 @klk - CC BY-NC-SA 3.0

#include "../common.hpp"

namespace sdf::manufactured {

namespace detail {
    inline float sq(float x) { return x * x; }
    
    inline float teapot_smin(float a, float b, float k) {
        float h = clamp(0.5f + 0.5f * (b - a) / k, 0.0f, 1.0f);
        return mix(b, a, h) - k * h * (1.0f - h);
    }
    
    inline float teapot_smax(float a, float b, float k) {
        return -teapot_smin(-a, -b, k);
    }
    
    inline float teapot_smin(float a, float b) {
        return teapot_smin(a, b, 0.1f);
    }
    
    inline float teapot_smax(float a, float b) {
        return teapot_smax(a, b, 0.1f);
    }
    
    inline float Torus(float x, float y, float z, float R, float r) {
        vec2 xz = vec2(x, z);
        vec2 q = vec2(length(xz) - R, y);
        return length(q) - r;
    }
    
    inline float Torus(const vec3& p, float R, float r) {
        vec2 q = vec2(length(vec2(p.x, p.z)) - R, p.y);
        return length(q) - r;
    }
    
    inline float Lid(float x, float y, float z) {
        float v = std::sqrt(sq(x) + sq(y - 0.55f) + sq(z)) - 1.4f;
        v = teapot_smin(v, Torus(y - 2.0f, x, z, 0.2f, 0.08f), 0.1f);
        v = teapot_smax(v, -std::sqrt(sq(x) + sq(y - 0.55f) + sq(z)) + 1.3f);
        v = teapot_smax(v, std::sqrt(sq(x) + sq(y - 2.5f) + sq(z)) - 1.3f);
        
        v = teapot_smax(v, -std::sqrt(sq(x - 0.25f) + sq(z - 0.35f)) + 0.05f, 0.05f);
        v = teapot_smin(v, Torus(x, (y - 1.45f) * 0.75f, z, 0.72f, 0.065f), 0.2f);
        return v;
    }
    
    inline float Nose(float x, float y, float z) {
        z -= std::sin((y + 0.8f) * 3.6f) * 0.15f;
        
        float v = std::sqrt(sq(x) + sq(z));
        
        v = std::abs(v - 0.3f + std::sin(y * 1.6f + 0.5f) * 0.18f) - 0.05f;
        v = teapot_smax(v, -y - 1.0f);
        v = teapot_smax(v, y - 0.85f, 0.075f);
        
        return v;
    }
    
    inline float Teapot(const vec3& p) {
        float x = p.x;
        float y = p.y;
        float z = p.z;
        
        float v = std::sqrt(x * x + z * z) - 1.2f - std::sin(y * 1.5f + 2.0f) * 0.4f;
        v = teapot_smax(v, std::abs(y) - 1.0f, 0.3f);
        
        float v1 = std::sqrt(x * x * 4.0f + sq(y + z * 0.1f) * 1.6f + sq(z + 1.2f)) - 1.0f;
        v1 = teapot_smax(v1, -std::sqrt(sq(z + 1.2f) + sq(y + z * 0.12f + 0.015f) * 1.8f) + 0.8f, 0.3f);
        
        v = teapot_smin(v, Torus(y * 1.2f + 0.2f + z * 0.3f, x * 0.75f, z + 1.25f + y * 0.2f, 0.8f, 0.1f), 0.25f);
        v = teapot_smin(v, std::sqrt(sq(x) + sq(y - 1.1f) + sq(z + 1.8f)) - 0.05f, 0.32f);
        
        float v3 = Nose(x, (y + z) * std::sqrt(0.5f) - 1.6f, (z - y) * std::sqrt(0.5f) - 1.1f);
        
        v = teapot_smin(v, v3, 0.2f);
        
        v = teapot_smax(v, teapot_smin(std::sin(y * 1.4f + 2.0f) * 0.5f + 0.95f - std::sqrt(x * x + z * z), y + 0.8f, 0.2f));
        v = teapot_smax(v, -std::sqrt(sq(x) + sq(y + 0.15f) + sq(z - 1.5f)) + 0.12f);
        
        v = teapot_smin(v, Torus(x, y - 0.95f, z, 0.9f, 0.075f));
        v = teapot_smin(v, Torus(x, y + 1.05f, z, 1.15f, 0.05f), 0.15f);
        
        float v2 = Lid(x, y + 0.5f, z);
        v = std::min(v, v2);
        
        return v;
    }
}

inline float Teapot(const vec3& p, float /*time*/, uint32_t /*seed*/) {
    const float scale = 0.3f;
    return detail::Teapot(p * (1.0f / scale)) * scale * 0.8f;
}

} // namespace sdf::manufactured


