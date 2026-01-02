#pragma once
// Ported from HumanHead.glsl
// Copyright 2019 Thomas Hooper @tdhooper - CC BY-NC-SA 3.0

#include "../common.hpp"

namespace sdf::animal {

namespace humanhead_detail {

inline void pR(float& px, float& py, float a) {
    float c = cos(a), s = sin(a);
    float nx = c * px + s * py;
    float ny = c * py - s * px;
    px = nx; py = ny;
}

inline vec2 pRi(vec2 p, float a) {
    pR(p.x, p.y, a);
    return p;
}

inline float vmax(const vec2& v) { return glm::max(v.x, v.y); }
inline float vmax(const vec3& v) { return glm::max(glm::max(v.x, v.y), v.z); }
inline float vmin(const vec3& v) { return glm::min(glm::min(v.x, v.y), v.z); }
inline float vmin(const vec2& v) { return glm::min(v.x, v.y); }

inline float fBox(const vec3& p, const vec3& b) {
    vec3 d = abs(p) - b;
    return length(max(d, vec3(0.0f))) + vmax(min(d, vec3(0.0f)));
}

inline float fCorner2(const vec2& p) {
    return length(max(p, vec2(0.0f))) + vmax(min(p, vec2(0.0f)));
}

inline float fDisc(const vec3& p, float r) {
    float l = length(vec2(p.x, p.z)) - r;
    return l < 0.0f ? abs(p.y) : length(vec2(p.y, l));
}

inline float fHalfCapsule(const vec3& p, float r) {
    return mix(length(vec2(p.x, p.z)) - r, length(p) - r, step(0.0f, p.y));
}

inline float sdRoundCone(const vec3& p, float r1, float r2, float h) {
    vec2 q = vec2(length(vec2(p.x, p.z)), p.y);
    float b = (r1 - r2) / h;
    float a = sqrt(1.0f - b * b);
    float k = dot(q, vec2(-b, a));
    if (k < 0.0f) return length(q) - r1;
    if (k > a * h) return length(q - vec2(0.0f, h)) - r2;
    return dot(q, vec2(a, b)) - r1;
}

inline float smin2(float a, float b, float r) {
    vec2 u = max(vec2(r - a, r - b), vec2(0.0f));
    return glm::max(r, glm::min(a, b)) - length(u);
}

inline float smax2(float a, float b, float r) {
    vec2 u = max(vec2(r + a, r + b), vec2(0.0f));
    return glm::min(-r, glm::max(a, b)) + length(u);
}

inline float head_smin(float a, float b, float k) {
    float f = clamp(0.5f + 0.5f * ((a - b) / k), 0.0f, 1.0f);
    return (1.0f - f) * a + f * b - f * (1.0f - f) * k;
}

inline float head_smax(float a, float b, float k) { return -head_smin(-a, -b, k); }

inline float smin3(float a, float b, float k) {
    return glm::min(head_smin(a, b, k), smin2(a, b, k));
}

inline float smax3(float a, float b, float k) {
    return glm::max(head_smax(a, b, k), smax2(a, b, k));
}

inline float ellip(const vec3& p, const vec3& s) {
    float r = vmin(s);
    return length(p * (r / s)) - r;
}

inline float ellip(const vec2& p, const vec2& s) {
    float r = vmin(s);
    return length(p * (r / s)) - r;
}

inline float mHead(vec3 p) {
    pR(p.y, p.z, -0.1f);
    p.y -= 0.11f;

    vec3 pa = p;
    p.x = abs(p.x);
    vec3 pp = p;

    float d = 1e12f;

    // skull back
    p += vec3(0.0f, -0.135f, 0.09f);
    d = ellip(p, vec3(0.395f, 0.385f, 0.395f));

    // skull base
    p = pp;
    p += vec3(0.0f, -0.135f, 0.09f) + vec3(0.0f, 0.1f, 0.07f);
    d = head_smin(d, ellip(p, vec3(0.38f, 0.36f, 0.35f)), 0.05f);

    // forehead
    p = pp;
    p += vec3(0.0f, -0.145f, -0.175f);
    d = head_smin(d, ellip(p, vec3(0.315f, 0.3f, 0.33f)), 0.18f);

    p = pp;
    pR(p.y, p.z, -0.5f);
    float bb = fBox(p, vec3(0.5f, 0.67f, 0.7f));
    d = head_smax(d, bb, 0.2f);

    // face base
    p = pp;
    p += vec3(0.0f, 0.25f, -0.13f);
    d = head_smin(d, length(p) - 0.28f, 0.1f);

    // behind ear
    p = pp;
    p += vec3(-0.15f, 0.13f, 0.06f);
    d = head_smin(d, ellip(p, vec3(0.15f, 0.15f, 0.15f)), 0.15f);

    p = pp;
    p += vec3(-0.07f, 0.18f, 0.1f);
    d = head_smin(d, length(p) - 0.2f, 0.18f);

    // cheek base
    p = pp;
    p += vec3(-0.2f, 0.12f, -0.14f);
    d = head_smin(d, ellip(p, vec3(0.15f, 0.22f, 0.2f) * 0.8f), 0.15f);

    // jaw base
    p = pp;
    p += vec3(0.0f, 0.475f, -0.16f);
    pR(p.y, p.z, 0.8f);
    d = head_smin(d, ellip(p, vec3(0.19f, 0.1f, 0.2f)), 0.1f);

    // brow
    p = pp;
    p += vec3(0.0f, -0.0f, -0.18f);
    vec3 bp = p;
    float brow = length(p) - 0.36f;
    p.x -= 0.37f;
    brow = head_smax(brow, dot(p, normalize(vec3(1.0f, 0.2f, -0.2f))), 0.2f);
    p = bp;
    brow = head_smax(brow, dot(p, normalize(vec3(0.0f, 0.6f, 1.0f))) - 0.43f, 0.25f);
    p = bp;
    pR(p.y, p.z, -0.5f);
    float peak = -p.y - 0.165f;
    peak += smoothstep(0.0f, 0.2f, p.x) * 0.01f;
    peak -= smoothstep(0.12f, 0.29f, p.x) * 0.025f;
    brow = head_smax(brow, peak, 0.07f);
    p = bp;
    pR(p.y, p.z, 0.5f);
    brow = head_smax(brow, -p.y - 0.06f, 0.15f);
    d = head_smin(d, brow, 0.06f);

    // nose
    p = pp;
    p += vec3(0.0f, 0.03f, -0.45f);
    pR(p.y, p.z, 3.0f);
    d = head_smin(d, sdRoundCone(p, 0.008f, 0.05f, 0.18f), 0.1f);

    p = pp;
    p += vec3(0.0f, 0.06f, -0.47f);
    pR(p.y, p.z, 2.77f);
    d = head_smin(d, sdRoundCone(p, 0.005f, 0.04f, 0.225f), 0.05f);

    // jaw
    p = pp;
    vec3 jo = vec3(-0.25f, 0.4f, -0.07f);
    p = pp + jo;
    float jaw = dot(p, normalize(vec3(1.0f, -0.2f, -0.05f))) - 0.069f;
    jaw = head_smax(jaw, dot(p, normalize(vec3(0.5f, -0.25f, 0.35f))) - 0.13f, 0.12f);
    jaw = head_smax(jaw, dot(p, normalize(vec3(-0.0f, -1.0f, -0.8f))) - 0.12f, 0.15f);
    jaw = head_smax(jaw, dot(p, normalize(vec3(0.98f, -1.0f, 0.15f))) - 0.13f, 0.08f);
    jaw = head_smax(jaw, dot(p, normalize(vec3(0.6f, -0.2f, -0.45f))) - 0.19f, 0.15f);
    jaw = head_smax(jaw, dot(p, normalize(vec3(0.5f, 0.1f, -0.5f))) - 0.26f, 0.15f);
    jaw = head_smax(jaw, dot(p, normalize(vec3(1.0f, 0.2f, -0.3f))) - 0.22f, 0.15f);

    p = pp;
    p += vec3(0.0f, 0.63f, -0.2f);
    pR(p.y, p.z, 0.15f);
    float cr = 0.5f;
    jaw = head_smax(jaw, length(vec2(p.x, p.y) - vec2(0.0f, cr)) - cr, 0.05f);

    p = pp + jo;
    jaw = head_smax(jaw, dot(p, normalize(vec3(0.0f, -0.4f, 1.0f))) - 0.35f, 0.1f);
    jaw = head_smax(jaw, dot(p, normalize(vec3(0.0f, 1.5f, 2.0f))) - 0.3f, 0.2f);
    jaw = glm::max(jaw, length(pp + vec3(0.0f, 0.6f, -0.3f)) - 0.7f);

    p = pa;
    p += vec3(0.2f, 0.5f, -0.1f);
    float jb = length(p);
    jb = smoothstep(0.0f, 0.4f, jb);
    float js = mix(0.0f, -0.005f, jb);
    jb = mix(0.01f, 0.04f, jb);

    d = head_smin(d, jaw - js, jb);

    // chin
    p = pp;
    p += vec3(0.0f, 0.585f, -0.395f);
    p.x *= 0.7f;
    d = head_smin(d, ellip(p, vec3(0.028f, 0.028f, 0.028f) * 1.2f), 0.15f);

    // cheek
    p = pp;
    p += vec3(-0.2f, 0.2f, -0.28f);
    pR(p.x, p.z, 0.5f);
    pR(p.y, p.z, 0.4f);
    float ch = ellip(p, vec3(0.1f, 0.1f, 0.12f) * 1.05f);
    d = head_smin(d, ch, 0.1f);

    p = pp;
    p += vec3(-0.26f, 0.02f, -0.1f);
    pR(p.x, p.z, 0.13f);
    pR(p.y, p.z, 0.5f);
    float temple = ellip(p, vec3(0.1f, 0.1f, 0.15f));
    temple = head_smax(temple, p.x - 0.07f, 0.1f);
    d = head_smin(d, temple, 0.1f);

    p = pp;
    p += vec3(0.0f, 0.2f, -0.32f);
    ch = ellip(p, vec3(0.1f, 0.08f, 0.1f));
    d = head_smin(d, ch, 0.1f);

    p = pp;
    p += vec3(-0.17f, 0.31f, -0.17f);
    ch = ellip(p, vec3(0.1f));
    d = head_smin(d, ch, 0.1f);

    // mouth base
    p = pp;
    p += vec3(-0.0f, 0.29f, -0.29f);
    pR(p.y, p.z, -0.3f);
    d = head_smin(d, ellip(p, vec3(0.13f, 0.15f, 0.1f)), 0.18f);

    p = pp;
    p += vec3(0.0f, 0.37f, -0.4f);
    d = head_smin(d, ellip(p, vec3(0.03f, 0.03f, 0.02f) * 0.5f), 0.1f);

    p = pp;
    p += vec3(-0.09f, 0.37f, -0.31f);
    d = head_smin(d, ellip(p, vec3(0.04f)), 0.18f);

    // bottom lip
    p = pp;
    p += vec3(0.0f, 0.455f, -0.455f);
    p.z += smoothstep(0.0f, 0.2f, p.x) * 0.05f;
    float lb = mix(0.035f, 0.03f, smoothstep(0.05f, 0.15f, length(p)));
    vec3 ls = vec3(0.055f, 0.028f, 0.022f) * 1.25f;
    float w = 0.192f;
    vec2 pl2 = vec2(p.x, length(vec2(p.y, p.z) * vec2(0.79f, 1.0f)));
    float bottomlip = length(pl2 + vec2(0.0f, w - ls.z)) - w;
    bottomlip = head_smax(bottomlip, length(pl2 - vec2(0.0f, w - ls.z)) - w, 0.055f);
    d = head_smin(d, bottomlip, lb);

    // top lip
    p = pp;
    p += vec3(0.0f, 0.38f, -0.45f);
    pR(p.x, p.z, -0.3f);
    ls = vec3(0.065f, 0.03f, 0.05f);
    w = ls.x * (-log(ls.y / ls.x) + 1.0f);
    vec3 pl = p * vec3(0.78f, 1.0f, 1.0f);
    float toplip = length(pl + vec3(0.0f, w - ls.y, 0.0f)) - w;
    toplip = head_smax(toplip, length(pl - vec3(0.0f, w - ls.y, 0.0f)) - w, 0.065f);
    p = pp;
    p += vec3(0.0f, 0.33f, -0.45f);
    pR(p.y, p.z, 0.7f);
    float cut = dot(p, normalize(vec3(0.5f, 0.25f, 0.0f))) - 0.056f;
    float dip = head_smin(dot(p, normalize(vec3(-0.5f, 0.5f, 0.0f))) + 0.005f,
                          dot(p, normalize(vec3(0.5f, 0.5f, 0.0f))) + 0.005f, 0.025f);
    cut = head_smax(cut, dip, 0.04f);
    cut = head_smax(cut, p.x - 0.1f, 0.05f);
    toplip = head_smax(toplip, cut, 0.02f);
    d = head_smin(d, toplip, 0.07f);

    // seam
    p = pp;
    p += vec3(0.0f, 0.425f, -0.44f);
    lb = length(p);
    float lr = mix(0.04f, 0.02f, smoothstep(0.05f, 0.12f, lb));
    pR(p.y, p.z, 0.1f);
    p.y -= smoothstep(0.0f, 0.03f, p.x) * 0.002f;
    p.y += smoothstep(0.03f, 0.1f, p.x) * 0.007f;
    p.z -= 0.133f;
    float seam = fDisc(p, 0.2f);
    seam = head_smax(seam, -d - 0.015f, 0.01f);
    d = mix(d, head_smax(d, -seam, lr), 0.65f);

    // nostrils base
    p = pp;
    p += vec3(0.0f, 0.3f, -0.43f);
    d = head_smin(d, length(p) - 0.05f, 0.07f);

    // nostrils
    p = pp;
    p += vec3(0.0f, 0.27f, -0.52f);
    pR(p.y, p.z, 0.2f);
    float nostrils = ellip(p, vec3(0.055f, 0.05f, 0.06f));

    p = pp;
    p += vec3(-0.043f, 0.28f, -0.48f);
    pR(p.x, p.y, 0.15f);
    p.z *= 0.8f;
    nostrils = head_smin(nostrils, sdRoundCone(p, 0.042f, 0.0f, 0.12f), 0.02f);
    d = head_smin(d, nostrils, 0.02f);

    p = pp;
    p += vec3(-0.033f, 0.3f, -0.515f);
    pR(p.x, p.z, 0.5f);
    d = head_smax(d, -ellip(p, vec3(0.011f, 0.03f, 0.025f)), 0.015f);

    // eyelids
    p = pp;
    p += vec3(-0.16f, 0.07f, -0.34f);
    float eyelids = ellip(p, vec3(0.08f, 0.1f, 0.1f));

    p = pp;
    p += vec3(-0.16f, 0.09f, -0.35f);
    float eyelids2 = ellip(p, vec3(0.09f, 0.1f, 0.07f));

    // edge top
    p = pp;
    p += vec3(-0.173f, 0.148f, -0.43f);
    p.x *= 0.97f;
    float et = length(vec2(p.x, p.y)) - 0.09f;

    // edge bottom
    p = pp;
    p += vec3(-0.168f, 0.105f, -0.43f);
    p.x *= 0.9f;
    float eb = dot(p, normalize(vec3(-0.1f, -1.0f, -0.2f))) + 0.001f;
    eb = head_smin(eb, dot(p, normalize(vec3(-0.3f, -1.0f, 0.0f))) - 0.006f, 0.01f);
    eb = head_smax(eb, dot(p, normalize(vec3(0.5f, -1.0f, -0.5f))) - 0.018f, 0.05f);

    float edge = glm::max(glm::max(eb, et), -d);

    d = head_smin(d, eyelids, 0.01f);
    d = head_smin(d, eyelids2, 0.03f);
    d = head_smax(d, -edge, 0.005f);

    // eyeball
    p = pp;
    p += vec3(-0.165f, 0.0715f, -0.346f);
    float eyeball = length(p) - 0.088f;
    d = glm::min(d, eyeball);

    // tear duct
    p = pp;
    p += vec3(-0.075f, 0.1f, -0.37f);
    d = glm::min(d, length(p) - 0.05f);

    // ear
    p = pp;
    p += vec3(-0.405f, 0.12f, 0.10f);
    pR(p.x, p.y, -0.12f);
    pR(p.x, p.z, 0.35f);
    pR(p.y, p.z, -0.3f);
    vec3 pe = p;

    // base
    float ear = p.x + smoothstep(-0.05f, 0.1f, p.y) * 0.015f - 0.005f;
    float earback = -ear - mix(0.001f, 0.025f, smoothstep(0.3f, -0.2f, p.y));

    // inner
    pR(p.x, p.z, -0.5f);
    float iear = ellip(vec2(p.z, p.y) - vec2(0.01f, -0.03f), vec2(0.045f, 0.05f));
    iear = head_smin(iear, length(vec2(p.z, p.y) - vec2(0.04f, -0.09f)) - 0.02f, 0.09f);
    float ridge = iear;
    iear = head_smin(iear, length(vec2(p.z, p.y) - vec2(0.1f, -0.03f)) - 0.06f, 0.07f);
    ear = smax2(ear, -iear, 0.04f);
    earback = head_smin(earback, iear - 0.04f, 0.02f);

    // ridge
    p = pe;
    pR(p.x, p.z, 0.2f);
    ridge = ellip(vec2(p.z, p.y) - vec2(0.01f, -0.03f), vec2(0.045f, 0.055f));
    ridge = smin3(ridge, -pRi(vec2(p.z, p.y), 0.2f).x - 0.01f, 0.015f);
    ridge = smax3(ridge, -ellip(vec2(p.z, p.y) - vec2(-0.01f, 0.1f), vec2(0.12f, 0.08f)), 0.02f);

    float ridger = 0.01f;
    ridge = glm::max(-ridge, ridge - ridger);
    ridge = smax2(ridge, abs(p.x) - ridger / 2.0f, ridger / 2.0f);
    ear = head_smin(ear, ridge, 0.045f);

    p = pe;

    // outline
    float outline = ellip(pRi(vec2(p.y, p.z), 0.2f), vec2(0.12f, 0.09f));
    outline = head_smin(outline, ellip(vec2(p.y, p.z) + vec2(0.155f, -0.02f), vec2(0.035f, 0.03f)), 0.14f);

    // edge
    float eedge = p.x + smoothstep(0.2f, -0.4f, p.y) * 0.06f - 0.03f;

    float edgeo = ellip(pRi(vec2(p.y, p.z), 0.1f), vec2(0.095f, 0.065f));
    edgeo = head_smin(edgeo, length(vec2(p.z, p.y) - vec2(0.0f, -0.1f)) - 0.03f, 0.1f);
    float edgeoin = head_smax(abs(pRi(vec2(p.z, p.y), 0.15f).y + 0.035f) - 0.01f, -p.z - 0.01f, 0.01f);
    edgeo = head_smax(edgeo, -edgeoin, 0.05f);

    float eedent = smoothstep(-0.05f, 0.05f, -p.z) * smoothstep(0.06f, 0.0f, fCorner2(vec2(-p.z, p.y)));
    eedent += smoothstep(0.1f, -0.1f, -p.z) * 0.2f;
    eedent += smoothstep(0.1f, -0.1f, p.y) * smoothstep(-0.03f, 0.0f, p.z) * 0.3f;
    eedent = glm::min(eedent, 1.0f);
    eedge += eedent * 0.06f;
    eedge = head_smax(eedge, -edgeo, 0.01f);
    ear = head_smin(ear, eedge, 0.01f);
    ear = glm::max(ear, earback);
    ear = smax2(ear, outline, 0.015f);
    d = head_smin(d, ear, 0.015f);

    // tragus
    p = pp;
    p += vec3(-0.34f, 0.2f, 0.02f);
    d = smin2(d, ellip(p, vec3(0.015f, 0.025f, 0.015f)), 0.035f);
    p = pp;
    p += vec3(-0.37f, 0.18f, 0.03f);
    pR(p.x, p.z, 0.5f);
    pR(p.y, p.z, -0.4f);
    d = head_smin(d, ellip(p, vec3(0.01f, 0.03f, 0.015f)), 0.015f);

    return d;
}

} // namespace humanhead_detail

inline float HumanHead(const vec3& p_in, float /*time*/, uint32_t /*seed*/) {
    vec3 p = p_in;
    const float scale = 1.0f;
    p /= scale;
    return humanhead_detail::mHead(p) * scale;
}

} // namespace sdf::animal

