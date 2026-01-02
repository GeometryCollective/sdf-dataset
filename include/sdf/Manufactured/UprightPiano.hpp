#pragma once
// Ported from UprightPiano.glsl
// Copyright 2013 Inigo Quilez @iq - CC BY-NC-SA 3.0

#include "../common.hpp"

namespace sdf::manufactured {

namespace uprightpiano_detail {

inline float sdCylinder(const vec3& p, const vec2& h) {
    vec2 d = abs(vec2(length(vec2(p.x, p.z)), p.y)) - h;
    return glm::min(glm::max(d.x, d.y), 0.0f) + length(max(d, vec2(0.0f)));
}

inline float sdBox(const vec3& p, const vec3& b) {
    vec3 d = abs(p) - b;
    return glm::min(glm::max(d.x, glm::max(d.y, d.z)), 0.0f) + length(max(d, vec3(0.0f)));
}

inline float udRoundBox(const vec3& p, const vec3& b, const vec3& r) {
    return length(max(abs(p) - b, vec3(0.0f))) - r.x;
}

inline float opRepLim(float p, float s, float mi, float ma) {
    return p - s * clamp(round(p / s), mi, ma);
}

inline float obj1(const vec3& p) {
    vec3 q = vec3(opRepLim(p.x + 0.1f, 0.2f, -22.0f, 23.0f), p.y - 0.1f, p.z - 0.1f);
    return udRoundBox(q, vec3(0.091f, 0.075f, 0.6f) - 0.005f, vec3(0.01f));
}

inline float obj2(const vec3& p, float d) {
    vec3 q = vec3(opRepLim(p.x, 0.2f, -21.0f, 23.0f), p.y - 0.185f, p.z - 0.3f);
    float k = mod(round(p.x / 0.2f), 7.0f);
    if (k == 2.0f || k == 6.0f) return d;
    return udRoundBox(q, vec3(0.06f, 0.075f, 0.4f) - 0.01f, vec3(0.01f, 0.01f, 0.01f));
}

inline float obj3(const vec3& p) {
    float d1 = udRoundBox(p - vec3(0.0f, 0.0f, 1.7f), vec3(5.4f, 0.6f, 1.0f), vec3(0.05f));
    float d2 = udRoundBox(p - vec3(0.0f, -0.3f, 0.1f), vec3(5.4f, 0.3f, 0.6f), vec3(0.05f));
    float d3 = udRoundBox(p - vec3(0.0f, -1.0f, 2.5f), vec3(5.4f, 3.0f, 1.0f), vec3(0.05f));

    float d4 = sdCylinder(vec3(abs(p.x), p.y, p.z) - vec3(5.25f, -2.2f, -0.35f), vec2(0.1f, 1.85f));
    d4 -= 0.03f * smoothstep(-0.7f, 0.7f, sin(18.0f * p.y)) + 0.017f * p.y + 0.025f;

    float d5 = udRoundBox(vec3(abs(p.x), p.y, p.z) - vec3(5.05f, 0.0f, 0.3f), vec3(0.35f, 0.2f, 0.8f), vec3(0.05f));

    return glm::min(glm::min(glm::min(glm::min(d1, d2), d3), d4), d5);
}

inline float obj6(const vec3& p) {
    vec3 q = p - vec3(0.0f, 1.3f, 1.1f);
    float x = abs(q.x);
    q.z += 0.15f * 4.0f * x * (1.0f - x);
    mat2 rm = mat2(0.9f, -0.43f, 0.43f, 0.9f);
    vec2 qyz = rm * vec2(q.y, q.z);
    q.y = qyz.x; q.z = qyz.y;
    return 0.5f * udRoundBox(q, vec3(1.0f, 0.7f, 0.0f), vec3(0.01f));
}

inline float obj8(const vec3& p) {
    vec3 q = p - vec3(-0.5f, -1.8f, -2.0f);
    mat2 rm = mat2(0.9f, 0.44f, -0.44f, 0.9f);
    vec2 qxz = rm * vec2(q.x, q.z);
    q.x = qxz.x; q.z = qxz.y;

    float y = 0.5f + 0.5f * sin(8.0f * q.x) * sin(8.0f * q.z);
    y = 0.1f * pow(y, 3.0f) * smoothstep(0.1f, 0.4f, q.y);
    float d = udRoundBox(q, vec3(1.5f, 0.25f, 0.6f), vec3(0.3f));
    d += y;

    vec3 s = vec3(abs(q.x), q.y, abs(q.z));
    float d2 = sdCylinder(s - vec3(1.4f, -1.2f, 0.6f), vec2(0.15f, 1.05f));
    return glm::min(d, d2);
}

inline float obj7(const vec3& p) {
    vec3 q = p - vec3(1.0f, -3.6f, 1.2f);
    vec3 r = vec3(mod(q.x - 0.25f, 0.5f) - 0.25f, q.y, q.z);
    return glm::max(0.5f * udRoundBox(r, vec3(0.05f, 0.0f, 0.38f), vec3(0.08f)), sdBox(q, vec3(0.75f, 1.0f, 1.0f)));
}

inline vec2 upright_map(const vec3& p) {
    // white keys
    vec2 res = vec2(obj1(p), 0.0f);

    // black keys
    vec2 ob2 = vec2(obj2(p, res.x), 1.0f);
    if (ob2.x < res.x) res = ob2;

    // piano body
    vec2 ob3 = vec2(obj3(p), 2.0f);
    if (ob3.x < res.x) res = ob3;

    // paper
    vec2 ob6 = vec2(obj6(p), 5.0f);
    if (ob6.x < res.x) res = ob6;

    // pedals
    vec2 ob7 = vec2(obj7(p), 6.0f);
    if (ob7.x < res.x) res = ob7;

    // bench
    vec2 ob8 = vec2(obj8(p), 7.0f);
    if (ob8.x < res.x) res = ob8;

    return res;
}

} // namespace uprightpiano_detail

inline float UprightPiano(const vec3& p_in, float /*time*/, uint32_t /*seed*/) {
    vec3 p = p_in;
    p = p * rotationMatrix(vec3(0.0f, 1.0f, 0.0f), pi);
    const float scale = 0.14f;
    p *= 1.0f / scale;
    return uprightpiano_detail::upright_map(p).x * scale;
}

} // namespace sdf::manufactured

