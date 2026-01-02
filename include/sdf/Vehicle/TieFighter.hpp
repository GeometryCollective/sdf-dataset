#pragma once
// Ported from TieFighter.glsl
// Copyright 2015 Nimitz @nimitz - CC BY-NC-SA 3.0

#include "../common.hpp"

namespace sdf::vehicle {

namespace tiefighter_detail {

inline float tri(float x) { return abs(fract(x) - 0.5f); }

inline mat3 rot_x(float a) {
    float sa = sin(a);
    float ca = cos(a);
    return mat3(1.0f, 0.0f, 0.0f, 0.0f, ca, sa, 0.0f, -sa, ca);
}

inline mat3 rot_y(float a) {
    float sa = sin(a);
    float ca = cos(a);
    return mat3(ca, 0.0f, sa, 0.0f, 1.0f, 0.0f, -sa, 0.0f, ca);
}

inline mat3 rot_z(float a) {
    float sa = sin(a);
    float ca = cos(a);
    return mat3(ca, sa, 0.0f, -sa, ca, 0.0f, 0.0f, 0.0f, 1.0f);
}

inline vec2 foldHex(vec2 p) {
    p.x = abs(p.x);
    p.y = abs(p.y);
    const vec2 pl1 = vec2(-0.5f, 0.8657f);
    const vec2 pl2 = vec2(-0.8657f, 0.5f);
    p -= pl1 * 2.0f * glm::min(0.0f, dot(p, pl1));
    p -= pl2 * 2.0f * glm::min(0.0f, dot(p, pl2));
    return p;
}

inline vec2 foldOct(vec2 p) {
    p.x = abs(p.x);
    p.y = abs(p.y);
    const vec2 pl1 = vec2(-0.7071f, 0.7071f);
    const vec2 pl2 = vec2(-0.9237f, 0.3827f);
    p -= pl1 * 2.0f * glm::min(0.0f, dot(p, pl1));
    p -= pl2 * 2.0f * glm::min(0.0f, dot(p, pl2));
    return p;
}

inline float sbox(const vec3& p, const vec3& b) {
    vec3 d = abs(p) - b;
    return glm::min(glm::max(d.x, glm::max(d.y, d.z)), 0.0f) + length(max(d, vec3(0.0f)));
}

inline float cyl(const vec3& p, const vec2& h) {
    vec2 d = abs(vec2(length(vec2(p.x, p.z)), p.y)) - h;
    return glm::min(glm::max(d.x, d.y), 0.0f) + length(max(d, vec2(0.0f)));
}

inline float torus(const vec3& p, const vec2& t) {
    return length(vec2(length(vec2(p.x, p.z)) - t.x, p.y)) - t.y;
}

inline float smoothfloor(float x, float k) {
    float xk = x + k * 0.5f;
    return floor(xk - 1.0f) + smoothstep(0.0f, k, fract(xk));
}

inline float hexprism(const vec3& p, const vec2& h) {
    vec3 q = abs(p);
    return glm::max(q.z - h.y, glm::max((q.y * 0.866025f + q.x * 0.5f), q.x) - h.x);
}

inline float map(const vec3& p_in) {
    vec3 bp = p_in;
    
    float mn = length(bp) - 0.7f;
    
    // Cockpit
    vec3 p = p_in;
    p.z -= 0.8f;
    vec3 q = p;
    
    // pi/8 rotation
    float c = 0.9239f, s = 0.3827f;
    mat2 rot = mat2(c, s, -s, c);
    vec2 qxy = rot * vec2(q.x, q.y);
    q.x = qxy.x;
    q.y = qxy.y;
    
    q.x = abs(q.x);
    q.y = abs(q.y);
    vec2 qxy_oct = foldOct(vec2(q.x, q.y));
    q.x = qxy_oct.x;
    q.y = qxy_oct.y;
    
    p.z += length(vec2(p.x, p.y)) * 0.46f;
    vec2 pxy_oct = foldOct(vec2(p.x, p.y));
    p.x = pxy_oct.x;
    p.y = pxy_oct.y;
    
    float g = sbox(p - vec3(0.32f, 0.2f, 0.0f), vec3(0.3f, 0.3f, 0.04f));
    
    float mg = glm::min(mn, g);
    mn = glm::max(mn, -g);
    float g2 = sbox(q, vec3(0.45f, 0.15f, 0.17f));
    mn = glm::max(mn, -g2);
    
    mn = glm::min(mn, torus(vec3(bp.y, bp.z, bp.x) + vec3(0.0f, -0.545f, 0.0f), vec2(0.4f, 0.035f)));
    mn = glm::max(mn, -torus(bp + vec3(0.0f, -0.585f, 0.0f), vec2(0.41f, 0.03f)));
    
    // Engine
    mn = glm::max(mn, -(bp.z + 0.6f));
    vec3 pl = vec3(bp.x, bp.z, bp.y);
    pl = vec3(length(vec2(pl.x, pl.z)) - 0.33f, pl.y, atan(pl.z, pl.x));
    pl.y += 0.55f;
    mn = glm::min(mn, sbox(pl, vec3(0.29f + bp.z * 0.35f, 0.25f, 4.0f)));
    pl.z = fract(pl.z * 1.7f) - 0.5f;
    mn = glm::min(mn, sbox(pl + vec3(0.03f, 0.09f, 0.0f), vec3(0.05f, 0.1f, 0.2f)));
    
    p = bp;
    p.x = abs(p.x) - 1.1f;
    
    mn = glm::min(mn, cyl(vec3(p.x, p.z, p.y) - vec3(-0.87f, 0.43f, -0.48f), vec2(0.038f, 0.1f)));
    
    const float wd = 0.61f;
    const float wg = 1.25f;
    
    mn = glm::min(mn, cyl(vec3(p.y, p.x, p.z),
                   vec2(0.22f + smoothfloor((abs(p.x + 0.12f) - 0.15f) * 4.0f, 0.1f) * 0.04f, 0.6f)));
    
    vec3 pp = p;
    pp.y *= 0.95f;
    vec3 r = p;
    p.y *= 0.65f;
    p.z = abs(p.z);
    p.z -= 0.16f;
    q = p;
    r.y = abs(r.y) - 0.5f;
    
    mn = glm::min(mn, sbox(r - vec3(-0.3f, -0.37f, 0.0f),
                    vec3(0.35f, 0.12f - smoothfloor(r.x * 2.0f - 0.4f, 0.1f) * 0.1f * (-r.x * 1.7f),
                         0.015f - r.x * 0.15f)));
    mn = glm::min(mn, sbox(r - vec3(0.0f, -0.5f, 0.0f), vec3(0.6f, 0.038f, 0.18f + r.x * 0.5f)));
    
    vec2 pzy = foldHex(vec2(p.z, p.y)) - 0.5f;
    p.z = pzy.x;
    p.y = pzy.y;
    vec2 ppzy = foldHex(vec2(pp.z, pp.y)) - 0.5f;
    pp.z = ppzy.x;
    pp.y = ppzy.y;
    
    mn = glm::min(mn, sbox(p - vec3(wd, wg, 0.0f), vec3(0.05f, 0.01f, 0.6f)));
    
    vec2 qyz = foldHex(vec2(q.y, q.z)) - 0.5f;
    q.y = qyz.x;
    q.z = qyz.y;
    
    mn = glm::min(mn, sbox(q - vec3(wd, -0.495f - abs(q.x - wd) * 0.07f, 0.0f),
                    vec3(0.16f - q.z * 0.07f, 0.015f - q.z * 0.005f, wg + 0.27f)));
    mn = glm::min(mn, sbox(q - vec3(wd, -0.5f, 0.0f), vec3(0.12f - q.z * 0.05f, 0.04f, wg + 0.26f)));
    
    mn = glm::min(mn, sbox(pp - vec3(wd, -0.35f, 0.0f), vec3(0.12f, 0.35f, 0.5f)));
    mn = glm::min(mn, sbox(pp - vec3(wd, -0.35f, 0.0f),
                    vec3(0.15f + tri(pp.y * pp.z * 30.0f * tri(pp.y * 2.5f)) * 0.06f, 0.25f, 0.485f)));
    
    float wgn = sbox(p - vec3(wd, 0.0f, 0.0f), vec3(0.04f, wg, 1.0f));
    mn = glm::min(mn, wgn);
    
    // Engine port
    float ep = hexprism(bp + vec3(0.0f, 0.0f, 0.6f), vec2(0.15f, 0.02f));
    mn = glm::min(mn, ep);
    
    return mn;
}

} // namespace tiefighter_detail

inline float TieFighter(const vec3& p_in, float /*time*/, uint32_t /*seed*/) {
    const float scale = 0.3f;
    vec3 p = p_in * (1.0f / scale);
    return tiefighter_detail::map(p) * scale;
}

} // namespace sdf::vehicle

