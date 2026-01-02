#pragma once
// Ported from Snail.glsl
// Copyright 2015 Inigo Quilez @iq - CC BY-NC-SA 3.0

#include "../common.hpp"

namespace sdf::animal {

namespace snail_detail {

inline float sdCapsule(const vec3& p, const vec3& a, const vec3& b, float r) {
    vec3 pa = p - a, ba = b - a;
    float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0f, 1.0f);
    return length(pa - ba * h) - r;
}

inline float sdTorus(const vec3& p, const vec2& t) {
    return length(vec2(length(vec2(p.x, p.z)) - t.x, p.y)) - t.y;
}

inline float snail_smin(float a, float b, float k) {
    float h = clamp(0.5f + 0.5f * (b - a) / k, 0.0f, 1.0f);
    return mix(b, a, h) - k * h * (1.0f - h);
}

inline float snail_smax(float a, float b, float k) {
    float h = clamp(0.5f + 0.5f * (b - a) / k, 0.0f, 1.0f);
    return mix(a, b, h) + k * h * (1.0f - h);
}

inline vec3 snail_smax3(const vec3& a, const vec3& b, float k) {
    vec3 h = clamp(vec3(0.5f) + 0.5f * (b - a) / k, vec3(0.0f), vec3(1.0f));
    return mix(a, b, h) + k * h * (vec3(1.0f) - h);
}

inline float sdSphere(const vec3& p, const vec4& s) {
    return length(p - vec3(s.x, s.y, s.z)) - s.w;
}

inline float snail_sdEllipsoid(const vec3& p, const vec3& c, const vec3& r) {
    return (length((p - c) / r) - 1.0f) * glm::min(glm::min(r.x, r.y), r.z);
}

inline vec2 udSegment(const vec3& p, const vec3& a, const vec3& b) {
    vec3 pa = p - a, ba = b - a;
    float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0f, 1.0f);
    return vec2(length(pa - ba * h), h);
}

inline float det2(const vec2& a, const vec2& b) { return a.x * b.y - b.x * a.y; }

inline vec3 getClosest(const vec2& b0, const vec2& b1, const vec2& b2) {
    float a = det2(b0, b2);
    float b = 2.0f * det2(b1, b0);
    float d = 2.0f * det2(b2, b1);
    float f = b * d - a * a;
    vec2 d21 = b2 - b1;
    vec2 d10 = b1 - b0;
    vec2 d20 = b2 - b0;
    vec2 gf = 2.0f * (b * d21 + d * d10 + a * d20);
    gf = vec2(gf.y, -gf.x);
    vec2 pp = -f * gf / dot(gf, gf);
    vec2 d0p = b0 - pp;
    float ap = det2(d0p, d20);
    float bp = 2.0f * det2(d10, d0p);
    float t = clamp((ap + bp) / (2.0f * a + b + d), 0.0f, 1.0f);
    return vec3(mix(mix(b0, b1, t), mix(b1, b2, t), t), t);
}

inline vec4 sdBezier(const vec3& a, const vec3& b, const vec3& c, const vec3& p) {
    vec3 w = normalize(cross(c - b, a - b));
    vec3 u = normalize(c - b);
    vec3 v = cross(w, u);
    
    vec2 a2 = vec2(dot(a - b, u), dot(a - b, v));
    vec2 b2 = vec2(0.0f);
    vec2 c2 = vec2(dot(c - b, u), dot(c - b, v));
    vec3 p3 = vec3(dot(p - b, u), dot(p - b, v), dot(p - b, w));
    
    vec3 cp = getClosest(a2 - vec2(p3.x, p3.y), b2 - vec2(p3.x, p3.y), c2 - vec2(p3.x, p3.y));
    
    return vec4(sqrt(dot(vec2(cp.x, cp.y), vec2(cp.x, cp.y)) + p3.z * p3.z), cp.z, length(vec2(cp.x, cp.y)), p3.z);
}

inline float mapShell(const vec3& p_in, vec4& matInfo) {
    const float sc = 1.0f;
    vec3 p = p_in - vec3(0.05f, 0.12f, -0.09f);
    p *= sc;
    
    mat3 m = mat3(-0.6333234236f, -0.7332753384f, 0.2474039592f,
                   0.7738444477f, -0.6034162289f, 0.1924931824f,
                   0.0081370606f,  0.3133626215f, 0.9495986813f);
    vec3 q = m * p;
    
    const float b = 0.1759f;
    
    float r = length(vec2(q.x, q.y));
    float t = atan(q.y, q.x);
    
    float n = (log(r) / b - t) / (2.0f * pi);
    float nm = (log(0.11f) / b - t) / (2.0f * pi);
    n = glm::min(n, nm);
    
    float ni = floor(n);
    
    float r1 = exp(b * (t + 2.0f * pi * ni));
    float r2 = r1 * 3.019863f;
    
    float h1 = q.z + 1.5f * r1 - 0.5f;
    float d1 = sqrt((r1 - r) * (r1 - r) + h1 * h1) - r1;
    float h2 = q.z + 1.5f * r2 - 0.5f;
    float d2 = sqrt((r2 - r) * (r2 - r) + h2 * h2) - r2;
    
    float d, dx, dy;
    if (d1 < d2) { d = d1; dx = r1 - r; dy = h1; }
    else         { d = d2; dx = r2 - r; dy = h2; }
    
    matInfo = vec4(dx, dy, r / 0.4f, t / pi);
    
    vec3 s = q;
    q = q - vec3(0.34f, -0.1f, 0.03f);
    mat2 rot2d = mat2(0.8f, 0.6f, -0.6f, 0.8f);
    vec2 qxy = rot2d * vec2(q.x, q.y);
    q.x = qxy.x;
    q.y = qxy.y;
    d = snail_smin(d, sdTorus(q, vec2(0.28f, 0.05f)), 0.06f);
    d = snail_smax(d, -snail_sdEllipsoid(q, vec3(0.0f), vec3(0.24f, 0.36f, 0.24f)), 0.03f);
    d = snail_smax(d, -snail_sdEllipsoid(s, vec3(0.52f, 0.0f, 0.0f), vec3(0.42f, 0.23f, 0.5f)), 0.05f);
    
    return d / sc;
}

inline vec2 mapSnail(const vec3& p, vec4& matInfo, float time) {
    vec3 head = vec3(-0.76f, 0.6f, -0.3f);
    vec3 q = p - head;
    
    // body
    vec4 b1 = sdBezier(vec3(-0.13f, -0.65f, 0.0f), vec3(0.24f, 1.0f, 0.0f), head + vec3(0.04f, 0.01f, 0.0f), p);
    float d1 = b1.x;
    d1 -= smoothstep(0.0f, 0.2f, b1.y) * (0.16f - 0.07f * smoothstep(0.5f, 1.0f, b1.y));
    b1 = sdBezier(vec3(-0.085f, 0.0f, 0.0f), vec3(-0.1f, 0.85f, 0.0f), head + vec3(0.06f, -0.08f, 0.0f), p);
    float d2 = b1.x;
    d2 -= 0.1f - 0.06f * b1.y;
    d1 = snail_smin(d1, d2, 0.03f);
    matInfo.x = b1.y;
    matInfo.y = b1.z;
    matInfo.z = b1.w;
    
    d2 = sdSphere(q, vec4(0.0f, -0.06f, 0.0f, 0.085f));
    d1 = snail_smin(d1, d2, 0.03f);
    
    d1 = snail_smin(d1, sdSphere(p, vec4(0.05f, 0.52f, 0.0f, 0.13f)), 0.07f);
    
    mat2 rot2d = mat2(0.8f, 0.6f, -0.6f, 0.8f);
    vec2 qxz = rot2d * vec2(q.x, q.z);
    q.x = qxz.x;
    q.z = qxz.y;
    
    vec3 sq = vec3(q.x, q.y, abs(q.z));
    
    // top antenas
    vec3 af = 0.05f * sin(0.5f * time + vec3(0.0f, 1.0f, 3.0f) + vec3(2.0f, 1.0f, 0.0f) * sign(q.z));
    vec4 b2 = sdBezier(vec3(0.0f), vec3(-0.1f, 0.2f, 0.2f), vec3(-0.3f, 0.2f, 0.3f) + af, sq);
    float d3 = b2.x;
    d3 -= 0.03f - 0.025f * b2.y;
    d1 = snail_smin(d1, d3, 0.04f);
    d3 = sdSphere(sq, vec4(-0.3f, 0.2f, 0.3f, 0.016f) + vec4(af, 0.0f));
    d1 = snail_smin(d1, d3, 0.01f);
    
    // bottom antenas
    vec3 bf = 0.02f * sin(0.3f * time + vec3(4.0f, 1.0f, 2.0f) + vec3(3.0f, 0.0f, 1.0f) * sign(q.z));
    vec2 b3 = udSegment(sq, vec3(0.06f, -0.05f, 0.0f), vec3(-0.04f, -0.2f, 0.18f) + bf);
    d3 = b3.x;
    d3 -= 0.025f - 0.02f * b3.y;
    d1 = snail_smin(d1, d3, 0.06f);
    d3 = sdSphere(sq, vec4(-0.04f, -0.2f, 0.18f, 0.008f) + vec4(bf, 0.0f));
    d1 = snail_smin(d1, d3, 0.02f);
    
    // bottom
    vec3 pp = p - vec3(-0.17f, 0.15f, 0.0f);
    float co = 0.988771078f;
    float si = 0.149438132f;
    mat2 rot2d2 = mat2(co, -si, si, co);
    vec2 ppxy = rot2d2 * vec2(pp.x, pp.y);
    pp.x = ppxy.x;
    pp.y = ppxy.y;
    d1 = snail_smin(d1, snail_sdEllipsoid(pp, vec3(0.0f), vec3(0.084f, 0.3f, 0.15f)), 0.05f);
    d1 = snail_smax(d1, -snail_sdEllipsoid(pp, vec3(-0.08f, 0.0f, 0.0f), vec3(0.06f, 0.55f, 0.1f)), 0.02f);
    
    return vec2(d1, 1.0f);
}

inline vec2 mapOpaque(const vec3& p, vec4& matInfo, float time) {
    matInfo = vec4(0.0f);
    
    vec2 res = mapSnail(p, matInfo, time);
    
    vec4 tmpMatInfo;
    float d4 = mapShell(p, tmpMatInfo);
    if (d4 < res.x) { res = vec2(d4, 2.0f); matInfo = tmpMatInfo; }
    
    // plant
    vec4 b3 = sdBezier(vec3(-0.15f, -1.5f, 0.0f), vec3(-0.1f, 0.5f, 0.0f), vec3(-0.6f, 1.5f, 0.0f), p);
    d4 = b3.x;
    d4 -= 0.04f - 0.02f * b3.y;
    if (d4 < res.x) { res = vec2(d4, 3.0f); }
    
    return res;
}

} // namespace snail_detail

inline float Snail(const vec3& p_in, float time, uint32_t /*seed*/) {
    const float scale = 0.8f;
    vec3 p = p_in * (1.0f / scale);
    vec4 temp = vec4(0.0f);
    return snail_detail::mapOpaque(p, temp, time).x * scale;
}

} // namespace sdf::animal

