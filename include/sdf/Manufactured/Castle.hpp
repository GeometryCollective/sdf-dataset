#pragma once
// Ported from Castle.glsl
// Copyright 2020 @sukupaper - CC BY-NC-SA 3.0

#include "../common.hpp"

namespace sdf::manufactured {

namespace castle_detail {

inline mat2 rot(float a) { float c = cos(a); float s = sin(a); return mat2(c, s, -s, c); }
inline float castle_rand(const vec2& st) { return fract(sin(dot(st, vec2(12.9898f, 78.233f))) * 43758.585f); }

inline float sq(const vec2& p, const vec2& s, float r) {
    return length(max(abs(p) - s, vec2(0.0f))) - r;
}

inline float ext(float d, const vec3& p, float h) {
    vec2 w = vec2(d, abs(p.z) - h);
    return glm::min(glm::max(w.x, w.y), 0.0f) + length(max(w, vec2(0.0f)));
}

inline float smoothunion(float d1, float d2, float k) {
    float h = clamp(0.5f + 0.5f * (d2 - d1) / k, 0.0f, 1.0f);
    return mix(d2, d1, h) - k * h * (1.0f - h);
}

inline float sph(const vec3& p, float s) { return length(p) - s; }

inline float box(const vec3& p, const vec3& s, float r) {
    return length(max(abs(p) - s, vec3(0.0f))) - r;
}

inline float cyl2(const vec3& p, float h, float r) {
    return glm::max(length(glm::max(abs(p.y) - h, 0.0f)) - 0.01f, length(vec2(p.x, p.z)) - r) - 0.01f;
}

inline float hollowcyl(const vec3& p, float h, float r) {
    return glm::max(length(glm::max(abs(p.y) - h, 0.0f)) - 0.01f, abs(length(vec2(p.x, p.z)) - r)) - 0.01f;
}

inline float door(vec3 p, float s) {
    p *= s;
    float d2d = sq(vec2(p.x, p.y), vec2(-0.05f, 0.515f), 0.3f);
    float d = ext(d2d, p, 0.2f);
    return d / s;
}

inline float doors1(vec3 p, float s) {
    p *= s;
    float d = glm::max(box(p, vec3(0.9f, 1.0f, 0.15f), 0.01f), -door(p - vec3(0.0f, -1.3f, -0.525f), 0.4f));
    p.x = abs(p.x);
    d = glm::max(d, -door(p - vec3(0.3f, -0.5f, -0.1f), 1.2f));
    d = glm::max(d, -box(p - vec3(0.3f, -0.15f, 0.0f), vec3(0.04f, 0.08f, 0.2f), 0.001f));
    d = glm::max(d, -box(p - vec3(0.0f, 0.35f, 0.0f), vec3(0.04f, 0.08f, 0.2f), 0.001f));
    return d / s;
}

inline float towers1(vec3 p) {
    p.x -= 0.65f;
    float d = hollowcyl(p, 0.51f, 0.2f) - 0.004f * step(0.5f, fract(1.5f * p.y));
    vec3 p1 = p;
    p1.x = abs(p.x) - 0.02f;
    p1.z += 0.1f;
    d = glm::max(d, -box(p1, vec3(0.005f, 0.02f, 5.0f), 0.001f));
    d = glm::max(d, -box(p1 + vec3(0.0f, 0.325f, 0.0f), vec3(0.005f, 0.02f, 5.0f), 0.001f));
    d = glm::max(d, -box(p1 - vec3(0.0f, 0.325f, 0.0f), vec3(0.005f, 0.02f, 5.0f), 0.001f));
    p.y -= 0.5f;
    d = smoothunion(d, sph(p, 0.19f), 0.02f);
    return d;
}

inline float towersAndDoors(vec3 p, float s) {
    p *= s;
    p.x = abs(p.x) - 0.375f;
    float d1 = doors1(p * vec3(1.0f, 1.0f, -1.0f), 1.992f);
    float d2 = towers1(p);
    float d = glm::min(d1, d2);
    return ((d - 0.005f * smoothstep(0.45f, 0.455f, p.y)) + 0.0001f * castle_rand(floor(vec2(p.x, p.y) * vec2(0.75f, 1.0f) * 180.0f))) / s;
}

inline float buildings(vec3 p, float s) {
    float a = atan(p.x, p.z);
    float l = length(vec2(p.x, p.z));
    vec3 rP = vec3(fract(a * 3.03f * s) - 0.5f, fract(p.y * 13.0f) - 0.5f, l);
    float d = cyl2(p, 0.38f, 0.2f * s);
    d = glm::max(d, -box(rP, vec3(0.075f, 0.15f, 10.0f), 0.001f));
    d = glm::min(d, sph((p - vec3(0.0f, 0.4f, 0.0f)) * vec3(1.0f, 1.2f, 1.0f), 0.210f * s));
    return d;
}

inline float df(vec3 p, float time) {
    // Rotation
    mat2 rotzy = rot(cos(time * 0.2f) * 0.0f + pi * 0.25f * 0.0f - 0.2f);
    vec2 pzy = rotzy * vec2(p.z, p.y);
    p.z = pzy.x;
    p.y = pzy.y;
    
    mat2 rotxz = rot(time * 0.05f);
    vec2 pxz = rotxz * vec2(p.x, p.z);
    p.x = pxz.x;
    p.z = pxz.y;
    
    float a = atan(p.x, p.z);
    float l = length(vec2(p.x, p.z));
    vec3 rP = vec3(fract(a * 3.024f) - 0.5f, p.y, l - 2.5f);
    vec3 rPfloor = vec3(floor(a * 3.024f) - 0.5f, p.y, l - 2.5f);
    vec3 rPnorm = vec3(a, p.y, l - 2.5f);
    vec3 rP1 = vec3(fract((a + 0.2f) * 2.7058f) - 0.5f, p.y - 0.5f, l - 2.2f);
    vec3 rP2 = vec3(fract((a + 0.3f) * 2.285f) - 0.5f, p.y - 1.0f, l - 1.9f);
    
    float d = 10e9f;
    d = towersAndDoors(rP, 2.065f);
    d = glm::max(d, -rP.y - (0.25f - 0.01f * castle_rand(floor(vec2(rP.x, rP.z) * 150.0f)) - 
        0.04f * castle_rand(floor(vec2(rP.x, rP.z) * 20.0f + vec2(rPfloor.x, rPfloor.z) * 20.0f)) + 0.02f));
    d = glm::min(d, towersAndDoors(rP1, 2.065f));
    d = glm::min(d, towersAndDoors(rP2, 2.065f));
    
    // Grid structure below
    float div = 0.35f;
    vec3 pp;
    pp.y = rP.y;
    pp.x = mod(rP.x, div) - div * 0.5f;
    pp.z = mod(rP.z, div) - div * 0.5f;
    float dGrid = glm::max(box(pp, vec3(0.2f, 0.17f - 0.1f * castle_rand(floor(vec2(rP.x, rP.z) * 5.0f)), 0.2f), 0.01f),
                          -box(pp, vec3(0.15f, 1.0f, 0.15f), 0.01f));
    dGrid = glm::max(dGrid, sph(p, 2.45f));
    d = glm::min(d, dGrid);
    
    // Main sphere
    vec3 pSph = p;
    pSph.y += 0.18f;
    d = glm::min(d, glm::max(sph(pSph, 1.6f - 0.025f * castle_rand(floor(vec2(rPnorm.x, rPnorm.z) * 4.0f))), pSph.y - 0.44f));
    
    // Buildings
    p.y -= 0.25f;
    vec3 rP3 = vec3(fract(a * 1.1f) - 0.5f, p.y - 1.0f, l - 1.2f);
    p.y -= 0.1f;
    vec3 rP4 = vec3(fract((a * 1.6f + 1.0f) * 0.25f) * 2.0f - 0.5f, p.y - 1.0f, l - 0.8f);
    vec3 rP5 = vec3(fract(a * 0.7f) - 0.5f, p.y - 1.0f, l - 0.7f);
    p.y -= 0.1f;
    vec3 rP6 = vec3(fract(a * 0.5f) - 0.5f, p.y - 1.0f, l - 0.5f);
    d = glm::min(d, buildings(rP3, 0.4f));
    d = glm::min(d, buildings(rP4, 1.5f));
    d = glm::min(d, buildings(rP5, 0.8f));
    d = glm::min(d, buildings(rP6, 0.6f));
    
    // Huge tree
    float varTree = 12.0f + cos(time * 0.1f);
    p.y += pow(length(vec2(p.x, p.z)) * 0.51f, 4.0f);
    d = glm::min(d, sph((p - vec3(0.0f, 1.9f, 0.0f)) * vec3(1.0f, 2.0f, 1.0f) +
        fract(cos(p.x * varTree) + sin(p.y * varTree) + sin(p.y * varTree)) * 0.032f, 1.25f));
    
    return d;
}

} // namespace castle_detail

inline float Castle(const vec3& p_in, float time, uint32_t /*seed*/) {
    vec3 p = p_in + vec3(0.0f, 0.3f, 0.0f);
    const float scale = 0.3f;
    p *= 1.0f / scale;
    return castle_detail::df(p, time) * 0.25f * scale;
}

} // namespace sdf::manufactured

