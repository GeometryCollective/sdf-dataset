#pragma once
// Ported from Cables.glsl
// Copyright 2020 @yuntaRobo - CC BY-NC-SA 3.0

#include "../common.hpp"

namespace sdf::manufactured {

namespace cables_detail {

inline mat2 rotate2D(float rad) {
    float c = cos(rad);
    float s = sin(rad);
    return mat2(c, s, -s, c);
}

inline vec2 de(const vec3& p_in, float time) {
    float d = 100.0f;
    float a = 0.0f;
    
    vec3 p = p_in;
    
    // yz rotation
    mat2 rotyz = rotate2D(pi / 5.0f);
    vec2 pyz = rotyz * vec2(p.y, p.z);
    p.y = pyz.x;
    p.z = pyz.y;
    p.y -= 0.5f;
    
    // Reaction sphere
    vec3 reaction = vec3(cos(time), 0.0f, sin(time)) * 3.0f;
    p += exp(-length(reaction - p) * 1.0f) * normalize(reaction - p);
    
    // Cables
    float r = atan(p.z, p.x) * 3.0f;
    const int ite = 50;
    for (int i = 0; i < ite; i++) {
        r += 0.5f / float(ite) * twopi;
        float s = 0.5f + sin(float(i) * 1.618f * twopi) * 0.25f;
        s += sin(time + float(i)) * 0.1f;
        vec2 q = vec2(length(vec2(p.x, p.z)) + cos(r) * s - 3.0f, p.y + sin(r) * s);
        float dd = length(q) - 0.035f;
        a = dd < d ? float(i) : a;
        d = glm::min(d, dd);
    }
    
    // Sphere
    float dd = length(p - reaction) - 0.1f;
    a = dd < d ? 0.0f : a;
    d = glm::min(d, dd);
    
    return vec2(d, a);
}

} // namespace cables_detail

inline float Cables(const vec3& p_in, float time, uint32_t /*seed*/) {
    const float scale = 0.23f;
    vec3 p = p_in * (1.0f / scale);
    return cables_detail::de(p, time).x * scale * 0.7f;
}

} // namespace sdf::manufactured

