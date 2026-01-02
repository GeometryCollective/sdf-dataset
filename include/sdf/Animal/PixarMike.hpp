#pragma once
// Ported from PixarMike.glsl
// Copyright 2013 Inigo Quilez @iq - CC BY-NC-SA 3.0
// Mike Wazowski from Monsters Inc

#include "../common.hpp"

namespace sdf::animal {

namespace pixarmike_detail {

inline vec2 sdSegment(const vec3& a, const vec3& b, const vec3& p) {
    vec3 pa = p - a, ba = b - a;
    float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0f, 1.0f);
    return vec2(length(pa - ba * h), h);
}

inline float sdEllipsoid(const vec3& p, const vec3& r) {
    return (length(p / r) - 1.0f) * glm::min(glm::min(r.x, r.y), r.z);
}

inline float mike_smin(float a, float b, float k) {
    float h = clamp(0.5f + 0.5f * (b - a) / k, 0.0f, 1.0f);
    return mix(b, a, h) - k * h * (1.0f - h);
}

inline float opS(float d1, float d2) { return glm::max(-d1, d2); }

inline vec2 map(const vec3& p_in) {
    vec3 p = p_in;
    p.y -= 1.8f;
    p.x = abs(p.x);

    vec3 q = p;
    q.y -= 0.3f * pow(1.0f - length(vec2(p.x, p.z)), 1.0f) * smoothstep(0.0f, 0.2f, p.y);
    q.y *= 1.05f;
    q.z *= 1.0f + 0.1f * smoothstep(0.0f, 0.5f, q.z) * smoothstep(-0.5f, 0.5f, p.y);
    float dd = length((p - vec3(0.0f, 0.65f, 0.8f)) * vec3(1.0f, 0.75f, 1.0f));
    float am = clamp(4.0f * abs(p.y - 0.45f), 0.0f, 1.0f);
    float fo = -0.03f * (1.0f - smoothstep(0.0f, 0.04f * am, abs(dd - 0.42f))) * am;
    float dd2 = length((p - vec3(0.0f, 0.65f, 0.8f)) * vec3(1.0f, 0.25f, 1.0f));
    float am2 = clamp(1.5f * (p.y - 0.45f), 0.0f, 1.0f);
    float fo2 = -0.085f * (1.0f - smoothstep(0.0f, 0.08f * am2, abs(dd2 - 0.42f))) * am2;
    q.y += -0.05f + 0.05f * length(q.x);

    float d1 = length(q) - 0.9f + fo + fo2;
    vec2 res = vec2(d1, 1.0f);

    // arms
    vec2 h = sdSegment(vec3(0.83f, 0.15f, 0.0f), vec3(1.02f, -0.6f, -0.1f), p);
    float d2 = h.x - 0.07f;
    res.x = mike_smin(res.x, d2, 0.03f);
    h = sdSegment(vec3(1.02f, -0.6f, -0.1f), vec3(0.95f, -1.2f, 0.1f), p);
    d2 = h.x - 0.07f + h.y * 0.02f;
    res.x = mike_smin(res.x, d2, 0.06f);

    // hands
    if (p.y < -1.0f) {
        float fa = sin(3.0f);
        h = sdSegment(vec3(0.95f, -1.2f, 0.1f), vec3(0.97f, -1.5f, 0.0f), p);
        d2 = h.x - 0.03f;
        res.x = mike_smin(res.x, d2, 0.01f);
        h = sdSegment(vec3(0.97f, -1.5f, 0.0f), vec3(0.95f, -1.7f, 0.0f) - 0.01f * fa, p);
        d2 = h.x - 0.03f + 0.01f * h.y;
        res.x = mike_smin(res.x, d2, 0.02f);
        h = sdSegment(vec3(0.95f, -1.2f, 0.1f), vec3(1.05f, -1.5f, 0.1f), p);
        d2 = h.x - 0.03f;
        res.x = mike_smin(res.x, d2, 0.02f);
        h = sdSegment(vec3(1.05f, -1.5f, 0.1f), vec3(1.0f, -1.75f, 0.1f) - 0.01f * fa, p);
        d2 = h.x - 0.03f + 0.01f * h.y;
        res.x = mike_smin(res.x, d2, 0.02f);
        h = sdSegment(vec3(0.95f, -1.2f, 0.1f), vec3(0.98f, -1.5f, 0.2f), p);
        d2 = h.x - 0.03f;
        res.x = mike_smin(res.x, d2, 0.03f);
        h = sdSegment(vec3(0.98f, -1.5f, 0.2f), vec3(0.95f, -1.7f, 0.15f) - 0.01f * fa, p);
        d2 = h.x - 0.03f + 0.01f * h.y;
        res.x = mike_smin(res.x, d2, 0.03f);
        h = sdSegment(vec3(0.95f, -1.2f, 0.1f), vec3(0.85f, -1.4f, 0.2f), p);
        d2 = h.x - 0.04f + 0.01f * h.y;
        res.x = mike_smin(res.x, d2, 0.05f);
        h = sdSegment(vec3(0.85f, -1.4f, 0.2f), vec3(0.85f, -1.63f, 0.15f) + 0.01f * fa, p);
        d2 = h.x - 0.03f + 0.01f * h.y;
        res.x = mike_smin(res.x, d2, 0.03f);
    }

    // legs
    if (p.y < 0.0f) {
        h = sdSegment(vec3(0.5f, -0.5f, 0.0f), vec3(0.6f, -1.2f, 0.1f), p);
        d2 = h.x - 0.14f + h.y * 0.08f;
        res.x = mike_smin(res.x, d2, 0.06f);
        h = sdSegment(vec3(0.6f, -1.2f, 0.1f), vec3(0.5f, -1.8f, 0.0f), p);
        d2 = h.x - 0.06f;
        res.x = mike_smin(res.x, d2, 0.06f);
    }

    // feet
    if (p.y < -1.5f) {
        h = sdSegment(vec3(0.5f, -1.8f, 0.0f), vec3(0.6f, -1.8f, 0.4f), p);
        d2 = h.x - 0.09f + 0.02f * h.y;
        res.x = mike_smin(res.x, d2, 0.06f);
        h = sdSegment(vec3(0.5f, -1.8f, 0.0f), vec3(0.77f, -1.8f, 0.35f), p);
        d2 = h.x - 0.08f + 0.02f * h.y;
        res.x = mike_smin(res.x, d2, 0.06f);
        h = sdSegment(vec3(0.5f, -1.8f, 0.0f), vec3(0.9f, -1.8f, 0.2f), p);
        d2 = h.x - 0.07f + 0.02f * h.y;
        res.x = mike_smin(res.x, d2, 0.06f);
    }

    // horns
    vec3 hp = p - vec3(0.25f, 0.7f, 0.0f);
    mat2 rm = mat2(0.6f, 0.8f, -0.8f, 0.6f);
    vec2 hpxy = rm * vec2(hp.x, hp.y);
    hp.x = hpxy.x; hp.y = hpxy.y;
    hp.x += 0.8f * hp.y * hp.y;
    float d4 = sdEllipsoid(hp, vec3(0.13f, 0.5f, 0.16f));
    if (d4 < res.x)
        res = vec2(d4, 3.0f);

    // eyes
    float d3 = length((p - vec3(0.0f, 0.25f, 0.35f)) * vec3(1.0f, 0.8f, 1.0f)) - 0.5f;
    if (d3 < res.x)
        res = vec2(d3, 2.0f);

    // mouth
    float mo = length((q - vec3(0.0f, -0.35f, 1.0f)) * vec3(1.0f, 1.2f, 0.25f) / 1.2f) - 0.3f / 1.2f;
    float of = 0.1f * pow(smoothstep(0.0f, 0.2f, abs(p.x - 0.3f)), 0.5f);
    mo = glm::max(mo, -q.y - 0.35f - of);

    float li = smoothstep(0.0f, 0.05f, mo + 0.02f) - smoothstep(0.05f, 0.10f, mo + 0.02f);
    res.x -= 0.03f * li * clamp((-q.y - 0.4f) * 10.0f, 0.0f, 1.0f);

    if (-mo > res.x)
        res = vec2(-mo, 4.0f);

    res.x += 0.01f * (smoothstep(0.0f, 0.05f, mo + 0.062f) -
                      smoothstep(0.05f, 0.10f, mo + 0.062f));

    // teeth
    if (p.x < 0.3f) {
        float px = mod(p.x, 0.16f) - 0.08f;
        float d5 = length((vec3(px, p.y, p.z) - vec3(0.0f, -0.37f, 0.65f)) * vec3(1.0f, 2.0f, 1.0f)) - 0.08f;
        if (d5 < res.x)
            res = vec2(d5, 2.0f);
    }

    return vec2(res.x * 0.8f, res.y);
}

} // namespace pixarmike_detail

inline float PixarMike(const vec3& p_in, float /*time*/, uint32_t /*seed*/) {
    vec3 p = p_in + vec3(0.0f, 0.5f, 0.0f);
    const float scale = 0.4f;
    p *= 1.0f / scale;
    return pixarmike_detail::map(p).x * scale;
}

} // namespace sdf::animal

