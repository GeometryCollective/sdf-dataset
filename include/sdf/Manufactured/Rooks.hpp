#pragma once
// Ported from Rooks.glsl
// Copyright 2020 @eiffie - CC BY-NC-SA 3.0

#include "../common.hpp"

namespace sdf::manufactured {

namespace rooks_detail {

inline float board(const vec2& v) {
    if (glm::max(abs(v.x), abs(v.y)) > 4.0f) return 0.0f;
    return 5.0f;
}

inline float DE(const vec3& p0) {
    vec3 p = vec3(fract(p0.x) - 0.5f, p0.y, fract(p0.z) - 0.5f);
    float id = board(vec2(p0.x, p0.z));
    float tp = abs(id);
    float mx = 0.65f - glm::max(abs(p.x), abs(p.z));
    if (tp == 0.0f) return mx;
    
    float f0 = 0.46f, f1 = 2.7f, f2 = 0.0f, f3 = 0.25f, f4 = 0.66f, f5 = -1.0f, f6 = 2.0f;
    float da = 1.0f, ds = 1.0f;
    
    // Rook piece
    p.y += 0.14f;
    f6 *= 1.5f;
    p *= f6;
    
    float r = length(vec2(p.x, p.z));
    if (p.y > 0.8f) {
        f5 = 1.0f;
        f0 = 0.0f;
        // Rook head
        f1 = 2.6f;
        f2 = 8.0f;
        f3 = 0.5f;
        f4 = 1.3f;
        ds = glm::max(-p.y + 1.0f, glm::min(r - 0.37f, glm::min(abs(p.x), abs(p.z)) - 0.09f));
    }
    
    float d = r - f0 + sin(p.y * f1 + f2) * f3;
    d = glm::max(d, p.y * f5 - f4);
    da = glm::min(da, length(max(vec2(r - 0.28f, abs(p.y - 0.8f)), vec2(0.0f))) - 0.05f);
    d = glm::max(glm::min(d, da), -ds);
    
    return glm::min(0.83f * d / f6, mx);
}

inline float sdBox(const vec3& p, const vec3& b) {
    vec3 d = abs(p) - b;
    return glm::min(glm::max(glm::max(d.x, d.y), d.z), 0.0f) + length(max(d, vec3(0.0f)));
}

} // namespace rooks_detail

inline float Rooks(const vec3& p_in, float /*time*/, uint32_t /*seed*/) {
    float boxD = rooks_detail::sdBox(p_in, vec3(1.0f, 1.0f, 1.0f));
    return glm::max(boxD, rooks_detail::DE(p_in)) * 0.5f;
}

} // namespace sdf::manufactured

