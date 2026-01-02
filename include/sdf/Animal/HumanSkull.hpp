#pragma once
// Ported from HumanSkull.glsl
// Copyright 2020 @monsterkodi - CC BY-NC-SA 3.0
// Skull and crossbones

#include "../common.hpp"

namespace sdf::animal {

namespace humanskull_detail {

const vec3 v0 = vec3(0.0f, 0.0f, 0.0f);
const vec3 vx = vec3(1.0f, 0.0f, 0.0f);
const vec3 vy = vec3(0.0f, 1.0f, 0.0f);
const vec3 vz = vec3(0.0f, 0.0f, 1.0f);

inline mat3 alignMatrix(const vec3& right, const vec3& up) {
    return mat3(right, up, cross(right, up));
}

inline float opUnion(float d1, float d2, float k) {
    float h = clamp(0.5f + 0.5f * (d2 - d1) / k, 0.0f, 1.0f);
    return mix(d2, d1, h) - k * h * (1.0f - h);
}

inline float opDiff(float d1, float d2, float k) {
    float h = clamp(0.5f - 0.5f * (d2 + d1) / k, 0.0f, 1.0f);
    return mix(d1, -d2, h) + k * h * (1.0f - h);
}

inline float opUnion(float d1, float d2) { return opUnion(d1, d2, 0.5f); }
inline float opDiff(float d1, float d2) { return opDiff(d1, d2, 0.0f); }

struct SDF {
    vec3 pos;
    float dist;
    int mat;
};

inline float sdSphere(const SDF& s, const vec3& a, float r) {
    return length(s.pos - a) - r;
}

inline float sdPlane(const SDF& s, const vec3& a, const vec3& n) {
    return dot(n, s.pos - a);
}

inline float sdPlane(const SDF& s, const vec3& n) {
    return dot(n, s.pos);
}

inline float sdBox(const SDF& s, const vec3& a, const vec3& b, float r) {
    vec3 q = abs(s.pos - a) - (b - r);
    return length(max(q, vec3(0.0f))) + glm::min(glm::max(q.x, glm::max(q.y, q.z)), 0.0f) - r;
}

inline float sdBox(const SDF& s, const vec3& a, const vec3& right, const vec3& up, const vec3& dim, float r) {
    SDF ss = s;
    ss.pos -= a;
    ss.pos = alignMatrix(right, up) * ss.pos;
    return sdBox(ss, v0, dim, r);
}

inline float sdEllipsoid(const SDF& s, const vec3& a, const vec3& r) {
    vec3 p = s.pos - a;
    float k0 = length(p / r);
    float k1 = length(p / (r * r));
    return k0 * (k0 - 1.0f) / k1;
}

inline float sdCone(const SDF& s, const vec3& a, float r1, float r2, float h) {
    vec3 p = s.pos - a;
    vec2 q = vec2(length(vec2(p.x, p.z)), p.y);
    float b = (r1 - r2) / h;
    float c = sqrt(1.0f - b * b);
    float k = dot(q, vec2(-b, c));
    if (k < 0.0f) return length(q) - r1;
    if (k > c * h) return length(q - vec2(0.0f, h)) - r2;
    return dot(q, vec2(c, b)) - r1;
}

inline float sdCapsule(const SDF& s, const vec3& a, const vec3& b, float r) {
    vec3 ab = b - a;
    vec3 ap = s.pos - a;
    float t = dot(ab, ap) / dot(ab, ab);
    t = clamp(t, 0.0f, 1.0f);
    vec3 c = a + t * ab;
    return length(s.pos - c) - r;
}

inline void skull(SDF& s) {
    s.pos.x = abs(s.pos.x);
    s.pos.y -= 0.15f;
    s.pos = alignMatrix(vx, normalize(vec3(0.0f, 1.0f, -0.5f))) * s.pos;

    float d, h;

    d = sdEllipsoid(s, vy, vec3(5.5f, 5.5f, 5.0f)); // frontal

    if (d > 15.0f) {
        s.dist = glm::min(s.dist, d);
        return;
    }

    d = opUnion(d, sdSphere(s, 2.0f * vy - 2.0f * vz, 6.0f), 1.0f);            // parietal
    d = opDiff(d, sdPlane(s, -vy, vy), 1.5f);                                   // cranial cutoff
    d = opUnion(d, sdCone(s, 4.1f * vz - 2.5f * vy, 2.5f, 1.8f, 3.5f), 0.5f);   // jaw
    d = opDiff(d, sdCone(s, 4.1f * vz - 2.5f * vy, 1.6f, 0.6f, 3.5f), 0.5f);    // jaw hole
    d = opDiff(d, sdCone(s, 5.8f * vz - 0.1f * vy, 1.0f, 0.5f, 1.5f), 0.3f);    // nose
    d = opDiff(d, sdPlane(s, -2.5f * vy, vy), 0.5f);                            // jaw cutoff
    d = opDiff(d, sdSphere(s, 2.7f * vx + 3.0f * vy + 3.6f * vz, 2.0f), 0.5f);  // eye holes

    d = opDiff(d, sdBox(s, 7.2f * vx + 3.5f * vy - 1.2f * vz, normalize(vec3(1.0f, -0.2f, 0.4f)), vy, vec3(2.0f, 3.0f, 3.0f), 1.0f), 1.0f);

    h = sdCapsule(s, -2.5f * vy - 1.5f * vz, -2.5f * vy - 0.2f * vz, 3.6f);
    h = opUnion(h, sdCapsule(s, vy - 2.0f * vz, vy - 0.5f * vz, 3.6f));
    d = opDiff(d, h, 1.0f);

    if (d < s.dist) {
        s.dist = d;
        s.mat = 0;
    }

    // teeth
    float td;
    td = sdBox(s, 0.47f * vx - 2.8f * vy + 6.1f * vz, normalize(vec3(1.0f, 0.0f, -0.2f)), vy, vec3(0.50f, 0.70f, 0.3f), 0.3f);
    if (td < s.dist) { s.dist = td; s.mat = 1; }
    td = sdBox(s, 1.29f * vx - 2.8f * vy + 5.7f * vz, normalize(vec3(1.0f, 0.0f, -0.8f)), vy, vec3(0.47f, 0.65f, 0.3f), 0.3f);
    if (td < s.dist) { s.dist = td; s.mat = 1; }
    td = sdBox(s, 1.80f * vx - 2.8f * vy + 5.0f * vz, normalize(vec3(0.4f, 0.0f, -1.0f)), vy, vec3(0.47f, 0.65f, 0.3f), 0.3f);
    if (td < s.dist) { s.dist = td; s.mat = 1; }
    td = sdBox(s, 2.00f * vx - 2.8f * vy + 4.1f * vz, normalize(vec3(0.0f, 0.0f, -1.0f)), vy, vec3(0.47f, 0.65f, 0.3f), 0.3f);
    if (td < s.dist) { s.dist = td; s.mat = 1; }
}

inline void bone(SDF& s) {
    s.pos.x = abs(s.pos.x);

    float d;
    vec3 ctr = 5.0f * vz - 1.8f * vy;
    vec3 rgt = 7.0f * vx + ctr + 3.0f * vz;
    d = sdCapsule(s, ctr, rgt, 0.9f);
    d = opUnion(d, sdSphere(s, rgt + vz, 1.7f), 0.5f);
    d = opUnion(d, sdSphere(s, rgt - vz - vx, 1.5f), 0.5f);

    rgt -= 6.0f * vz;
    rgt += (rgt - ctr) * 0.3f;
    d = glm::min(d, sdCapsule(s, ctr, rgt, 0.9f));
    d = opUnion(d, sdSphere(s, rgt - vz, 1.7f), 0.5f);
    d = opUnion(d, sdSphere(s, rgt + vz - vx, 1.5f), 0.5f);

    if (d < s.dist) {
        s.dist = d;
        s.mat = 2;
    }
}

inline float map(const vec3& p) {
    SDF s;
    s.dist = 10.0f;
    s.pos = p;
    s.mat = -1;

    bone(s);
    skull(s);

    return s.dist;
}

} // namespace humanskull_detail

inline float HumanSkull(const vec3& p_in, float /*time*/, uint32_t /*seed*/) {
    vec3 p = p_in + vec3(0.0f, 0.0f, 0.15f);
    const float scale = 0.075f;
    p *= 1.0f / scale;
    return humanskull_detail::map(p) * scale;
}

} // namespace sdf::animal

