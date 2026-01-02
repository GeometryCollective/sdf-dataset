#pragma once
// Ported from Elephant.glsl
// Copyright 2016 Inigo Quilez @iq - CC BY-NC-SA 3.0

#include "../common.hpp"

namespace sdf::animal {

namespace elephant_detail {

inline float hash1(float n) { return fract(sin(n) * 43758.5453123f); }

inline float eleph_smin(float a, float b, float k) {
    float h = clamp(0.5f + 0.5f * (b - a) / k, 0.0f, 1.0f);
    return mix(b, a, h) - k * h * (1.0f - h);
}

inline vec2 eleph_smin2(const vec2& a, const vec2& b, float k) {
    float h = clamp(0.5f + 0.5f * (b.x - a.x) / k, 0.0f, 1.0f);
    return vec2(mix(b.x, a.x, h) - k * h * (1.0f - h), mix(b.y, a.y, h));
}

inline float eleph_smax(float a, float b, float k) {
    float h = clamp(0.5f + 0.5f * (b - a) / k, 0.0f, 1.0f);
    return mix(a, b, h) + k * h * (1.0f - h);
}

inline vec2 sdSegment(const vec3& p, const vec3& a, const vec3& b) {
    vec3 pa = p - a, ba = b - a;
    float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0f, 1.0f);
    return vec2(length(pa - ba * h), h);
}

inline float sdSphere(const vec3& p, const vec3& c, float r) {
    return length(p - c) - r;
}

inline float sdEllipsoid(const vec3& p, const vec3& c, const vec3& r) {
    return (length((p - c) / r) - 1.0f) * glm::min(glm::min(r.x, r.y), r.z);
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

inline vec2 sdBezier(const vec3& a, const vec3& b, const vec3& c, const vec3& p, vec2& pos) {
    vec3 w = normalize(cross(c - b, a - b));
    vec3 u = normalize(c - b);
    vec3 v = normalize(cross(w, u));

    vec2 a2 = vec2(dot(a - b, u), dot(a - b, v));
    vec2 b2 = vec2(0.0f);
    vec2 c2 = vec2(dot(c - b, u), dot(c - b, v));
    vec3 p3 = vec3(dot(p - b, u), dot(p - b, v), dot(p - b, w));

    vec3 cp = getClosest(a2 - vec2(p3.x, p3.y), b2 - vec2(p3.x, p3.y), c2 - vec2(p3.x, p3.y));
    pos = vec2(cp.x, cp.y);

    return vec2(sqrt(dot(vec2(cp.x, cp.y), vec2(cp.x, cp.y)) + p3.z * p3.z), cp.z);
}

inline mat3 base(const vec3& ww) {
    vec3 vv = vec3(0.0f, 0.0f, 1.0f);
    vec3 uu = normalize(cross(vv, ww));
    return mat3(uu.x, ww.x, vv.x, uu.y, ww.y, vv.y, uu.z, ww.z, vv.z);
}

inline float leg(const vec3& p, const vec3& pa, const vec3& pb, const vec3& pc, float m, float h) {
    vec2 b = sdSegment(p, pa, pb);
    float tr = 0.35f - 0.16f * smoothstep(0.0f, 1.0f, b.y);
    float d3 = b.x - tr;

    b = sdSegment(p, pb, pc);
    tr = 0.18f;
    d3 = eleph_smin(d3, b.x - tr, 0.1f);

    // paw
    vec3 ww = normalize(mix(normalize(pc - pb), vec3(0.0f, 1.0f, 0.0f), h));
    mat3 pr = base(ww);
    vec3 fc = pr * (p - pc) - vec3(0.02f, 0.0f, 0.0f) * (-1.0f + 2.0f * h);
    float d4 = sdEllipsoid(fc, vec3(0.0f), vec3(0.2f, 0.15f, 0.2f));

    d3 = eleph_smin(d3, d4, 0.1f);

    // nails
    float d6 = sdEllipsoid(fc, vec3(0.14f, -0.06f, 0.0f) * (-1.0f + 2.0f * h), vec3(0.1f, 0.16f, 0.1f));
    d6 = glm::min(d6, sdEllipsoid(vec3(fc.x, fc.y, abs(fc.z)),
                           vec3(0.13f * (-1.0f + 2.0f * h), -0.08f * (-1.0f + 2.0f * h), 0.13f),
                           vec3(0.09f, 0.14f, 0.1f)));
    d3 = eleph_smin(d3, d6, 0.001f);
    return d3;
}

inline float mapElephant(vec3 p, vec3& matInfo) {
    matInfo = vec3(0.0f);

    p.x -= -0.5f;
    p.y -= 2.4f;

    vec3 ph = p;
    float cc = 0.995f;
    float ss = 0.0998745f;
    mat2 rot1 = mat2(cc, -ss, ss, cc);
    vec2 phyz = rot1 * vec2(ph.y, ph.z);
    ph.y = phyz.x; ph.z = phyz.y;
    vec2 phxy = rot1 * vec2(ph.x, ph.y);
    ph.x = phxy.x; ph.y = phxy.y;

    // head
    float d1 = sdEllipsoid(ph, vec3(0.0f, 0.05f, 0.0f), vec3(0.45f, 0.5f, 0.3f));
    d1 = eleph_smin(d1, sdEllipsoid(ph, vec3(-0.3f, 0.15f, 0.0f), vec3(0.2f, 0.2f, 0.2f)), 0.1f);

    // nose (trunk)
    vec2 kk;
    vec2 b1 = sdBezier(vec3(-0.15f, -0.05f, 0.0f), vec3(-0.7f, 0.0f, 0.0f),
                       vec3(-0.7f, -0.8f, 0.0f), ph, kk);
    float tr1 = 0.30f - 0.17f * smoothstep(0.0f, 1.0f, b1.y);
    vec2 b2 = sdBezier(vec3(-0.7f, -0.8f, 0.0f), vec3(-0.7f, -1.5f, 0.0f),
                       vec3(-0.4f, -1.6f, 0.2f), ph, kk);
    float tr2 = 0.30f - 0.17f - 0.05f * smoothstep(0.0f, 1.0f, b2.y);
    float bd1 = b1.x - tr1;
    float bd2 = b2.x - tr2;
    float nl = b1.y * 0.5f;
    float bd = bd1;
    if (bd2 < bd1) {
        nl = 0.5f + 0.5f * b2.y;
        bd = bd2;
    }
    matInfo.x = clamp(nl * (1.0f - smoothstep(0.0f, 0.2f, bd)), 0.0f, 1.0f);
    float d2 = bd;
    float xx = nl * 120.0f;
    float ff = sin(xx + sin(xx + sin(xx + sin(xx))));
    d2 += 0.003f * ff * (1.0f - nl) * (1.0f - nl) * smoothstep(0.0f, 0.1f, nl);

    float d = eleph_smin(d1, d2, 0.2f);

    // teeth (tusks)
    vec3 q = vec3(p.x, p.y, abs(p.z));
    vec3 qh = vec3(ph.x, ph.y, abs(ph.z));
    {
        vec2 s1 = sdSegment(qh, vec3(-0.4f, -0.1f, 0.1f), vec3(-0.5f, -0.4f, 0.28f));
        float d3 = s1.x - 0.18f * (1.0f - 0.3f * smoothstep(0.0f, 1.0f, s1.y));
        d = eleph_smin(d, d3, 0.1f);
    }

    // eyes
    {
        vec2 s1 = sdSegment(qh, vec3(-0.2f, 0.2f, 0.11f), vec3(-0.3f, -0.0f, 0.26f));
        float d3 = s1.x - 0.19f * (1.0f - 0.3f * smoothstep(0.0f, 1.0f, s1.y));
        d = eleph_smin(d, d3, 0.03f);

        float st = length(vec2(qh.x, qh.y) - vec2(-0.31f, -0.02f));
        d += 0.0015f * sin(250.0f * st) * (1.0f - smoothstep(0.0f, 0.2f, st));

        mat3 rot3 = mat3(0.8f, -0.6f, 0.0f, 0.6f, 0.8f, 0.0f, 0.0f, 0.0f, 1.0f);
        float d4 = sdEllipsoid(rot3 * (qh - vec3(-0.31f, -0.02f, 0.34f)), vec3(0.0f),
                               vec3(0.1f, 0.08f, 0.07f) * 0.7f);
        d = eleph_smax(d, -d4, 0.02f);
    }

    // body
    {
        float co = cos(0.4f);
        float si = sin(0.4f);
        vec3 w = p;
        mat2 rotw = mat2(co, si, -si, co);
        vec2 wxy = rotw * vec2(w.x, w.y);
        w.x = wxy.x; w.y = wxy.y;

        float d4 = sdEllipsoid(w, vec3(0.6f, 0.3f, 0.0f), vec3(0.6f, 0.6f, 0.6f));
        d = eleph_smin(d, d4, 0.1f);

        d4 = sdEllipsoid(w, vec3(1.8f, 0.3f, 0.0f), vec3(1.2f, 0.9f, 0.7f));
        d = eleph_smin(d, d4, 0.2f);

        d4 = sdEllipsoid(w, vec3(2.1f, 0.55f, 0.0f), vec3(1.0f, 0.9f, 0.6f));
        d = eleph_smin(d, d4, 0.1f);

        d4 = sdEllipsoid(w, vec3(2.0f, 0.8f, 0.0f), vec3(0.7f, 0.6f, 0.8f));
        d = eleph_smin(d, d4, 0.1f);
    }

    // back-left leg
    {
        float d3 = leg(q, vec3(2.6f, -0.5f, 0.3f), vec3(2.65f, -1.45f, 0.3f),
                       vec3(2.6f, -2.1f, 0.25f), 1.0f, 0.0f);
        d = eleph_smin(d, d3, 0.1f);
    }

    // tail
    {
        vec2 b = sdBezier(vec3(2.8f, 0.2f, 0.0f), vec3(3.4f, -0.6f, 0.0f),
                          vec3(3.1f, -1.6f, 0.0f), p, kk);
        float tr = 0.10f - 0.07f * b.y;
        float d_tail = b.x - tr;
        d = eleph_smin(d, d_tail, 0.05f);
    }

    // front legs
    {
        float d3 = leg(p, vec3(0.8f, -0.4f, 0.3f), vec3(0.7f, -1.55f, 0.3f),
                       vec3(0.8f, -2.1f, 0.3f), 1.0f, 0.0f);
        d = eleph_smin(d, d3, 0.15f);
        d3 = leg(p, vec3(0.8f, -0.4f, -0.3f), vec3(0.4f, -1.55f, -0.3f),
                 vec3(0.4f, -2.1f, -0.3f), 1.0f, 0.0f);
        d = eleph_smin(d, d3, 0.15f);
    }

    // ear
    float co = cos(0.5f);
    float si = sin(0.5f);
    vec3 w = qh;
    mat2 rotw = mat2(co, si, -si, co);
    vec2 wxz = rotw * vec2(w.x, w.z);
    w.x = wxz.x; w.z = wxz.y;

    vec2 ep = vec2(w.z, w.y) - vec2(0.5f, 0.4f);
    float aa = atan(ep.x, ep.y);
    w.x += 0.003f * sin(24.0f * aa) * smoothstep(0.0f, 0.5f, dot(ep, ep));

    float r = 0.02f * sin(24.0f * atan(ep.x, ep.y)) * clamp(-w.y * 1000.0f, 0.0f, 1.0f);
    r += 0.01f * sin(15.0f * w.z);
    
    float d4 = length(vec2(w.z, w.y) - vec2(0.5f, -0.2f + 0.03f)) - 0.8f + r;
    float d5 = length(vec2(w.z, w.y) - vec2(-0.1f, 0.6f + 0.03f)) - 1.5f + r;
    float d6 = length(vec2(w.z, w.y) - vec2(1.8f, 0.1f + 0.03f)) - 1.6f + r;
    d4 = eleph_smax(d4, d5, 0.1f);
    d4 = eleph_smax(d4, d6, 0.1f);

    float wi = 0.02f + 0.1f * pow(clamp(1.0f - 0.7f * w.z + 0.3f * w.y, 0.0f, 1.0f), 2.0f);
    w.x += 0.05f * cos(6.0f * w.y);

    d4 = eleph_smax(d4, -w.x, 0.03f);
    d4 = eleph_smax(d4, w.x - wi, 0.03f);

    matInfo.y = clamp(length(ep), 0.0f, 1.0f) * (1.0f - smoothstep(-0.1f, 0.05f, d4));

    d = eleph_smin(d, d4, 0.3f * glm::max(qh.y, 0.0f));

    // connection ear/head
    vec2 s1 = sdBezier(vec3(-0.15f, 0.3f, 0.0f), vec3(0.1f, 0.6f, 0.2f),
                       vec3(0.35f, 0.6f, 0.5f), qh, kk);
    float d3 = s1.x - 0.08f * (1.0f - 0.95f * s1.y * s1.y);
    d = eleph_smin(d, d3, 0.05f);

    // tusks
    vec2 b = sdBezier(vec3(-0.5f, -0.4f, 0.28f), vec3(-0.5f, -0.7f, 0.32f),
                      vec3(-1.0f, -0.8f, 0.45f), qh, kk);
    float tr = 0.10f - 0.08f * b.y;
    d2 = b.x - tr;
    
    // eyeball
    mat3 rot3 = mat3(0.8f, -0.6f, 0.0f, 0.6f, 0.8f, 0.0f, 0.0f, 0.0f, 1.0f);
    d4 = sdEllipsoid(rot3 * (qh - vec3(-0.31f, -0.02f, 0.33f)), vec3(0.0f),
                     vec3(0.1f, 0.08f, 0.07f) * 0.7f);

    d = glm::min(d4, glm::min(d, d2));

    return d;
}

} // namespace elephant_detail

inline float Elephant(const vec3& p_in, float /*time*/, uint32_t /*seed*/) {
    vec3 p = p_in;
    p = p * rotationMatrix(vec3(0.0f, 1.0f, 0.0f), -pi / 2.0f);
    p += vec3(0.2f, 0.4f, 0.0f);
    const float scale = 0.3f;
    p *= 1.0f / scale;
    vec3 matInfo = vec3(0.0f);
    return elephant_detail::mapElephant(p, matInfo) * scale * 0.9f;
}

} // namespace sdf::animal

