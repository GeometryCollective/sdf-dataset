#pragma once
// Ported from Temple.glsl
// Copyright 2017 Inigo Quilez @iq - CC BY-NC-SA 3.0

#include "../common.hpp"

namespace sdf::manufactured {

namespace temple_detail {

inline float hash11(float p) {
    return fract(sin(p * 727.1f) * 435.545f);
}

inline float hash12(const vec2& p) {
    float h = dot(p, vec2(127.1f, 311.7f));
    return fract(sin(h) * 437.545f);
}

inline vec3 hash31(float p) {
    vec3 h = vec3(127.231f, 491.7f, 718.423f) * p;
    return fract(sin(h) * 435.543f);
}

inline float noise_3(const vec3& p) {
    vec3 i = floor(p);
    vec3 f = fract(p);
    vec3 u = f * f * (3.0f - 2.0f * f);
    
    vec2 ii = vec2(i.x, i.y) + i.z * vec2(5.0f);
    float a = hash12(ii + vec2(0.0f, 0.0f));
    float b = hash12(ii + vec2(1.0f, 0.0f));
    float c = hash12(ii + vec2(0.0f, 1.0f));
    float d = hash12(ii + vec2(1.0f, 1.0f));
    float v1 = mix(mix(a, b, u.x), mix(c, d, u.x), u.y);
    
    ii += vec2(5.0f);
    a = hash12(ii + vec2(0.0f, 0.0f));
    b = hash12(ii + vec2(1.0f, 0.0f));
    c = hash12(ii + vec2(0.0f, 1.0f));
    d = hash12(ii + vec2(1.0f, 1.0f));
    float v2 = mix(mix(a, b, u.x), mix(c, d, u.x), u.y);
    
    return glm::max(mix(v1, v2, u.z), 0.0f);
}

inline float fbm3_high(const vec3& p, float a, float f) {
    float ret = 0.0f;
    float amp = 1.0f;
    float frq = 1.0f;
    for (int i = 0; i < 5; i++) {
        float n = pow(noise_3(p * frq), 2.0f);
        ret += n * amp;
        frq *= f;
        amp *= a * pow(n, 0.2f);
    }
    return ret;
}

inline float hash1(const vec2& p_in) {
    vec2 p = 50.0f * fract(p_in * 0.3183099f);
    return fract(p.x * p.y * (p.x + p.y));
}

inline float ndot(const vec2& a, const vec2& b) { return a.x * b.x - a.y * b.y; }

inline float sdRhombus(const vec2& p_in, const vec2& b, float r) {
    vec2 q = abs(p_in);
    float h = clamp((-2.0f * ndot(q, b) + ndot(b, b)) / dot(b, b), -1.0f, 1.0f);
    float d = length(q - 0.5f * b * vec2(1.0f - h, 1.0f + h));
    d *= sign(q.x * b.y + q.y * b.x - b.x * b.y);
    return d - r;
}

inline float usdBox(const vec3& p, const vec3& b) {
    return length(max(abs(p) - b, vec3(0.0f)));
}

inline float sdBox3(const vec3& p, const vec3& b) {
    vec3 d = abs(p) - b;
    return glm::min(glm::max(d.x, glm::max(d.y, d.z)), 0.0f) + length(max(d, vec3(0.0f)));
}

inline float sdBox1(float p, float b) {
    return abs(p) - b;
}

inline vec2 opRepLim2(const vec2& p, float s, const vec2& lim) {
    return p - s * clamp(round(p / s), -lim, lim);
}

inline vec2 opRepLim4(const vec2& p, float s, const vec2& limmin, const vec2& limmax) {
    return p - s * clamp(round(p / s), -limmin, limmax);
}

inline vec3 temple(const vec3& p_in) {
    vec3 op = p_in;
    vec3 res = vec3(-1.0f, -1.0f, 0.5f);
    
    vec3 p = p_in;
    p.y += 2.0f;
    
    float bbox = usdBox(p, vec3(15.0f, 12.0f, 15.0f) * 1.5f);
    if (bbox > 5.0f) return vec3(bbox + 1.0f, -1.0f, 0.5f);
    
    vec3 q = p;
    vec2 qxz = opRepLim2(vec2(q.x, q.z), 4.0f, vec2(4.0f, 2.0f));
    q.x = qxz.x;
    q.z = qxz.y;
    
    vec2 id = floor((vec2(p.x, p.z) + 2.0f) / 4.0f);
    
    float d = length(vec2(q.x, q.z)) - 0.9f + 0.05f * p.y;
    d = glm::max(d, p.y - 6.0f);
    d = glm::max(d, -p.y - 5.0f);
    d -= 0.05f * pow(0.5f + 0.5f * sin(atan(q.x, q.z) * 16.0f), 2.0f);
    d -= 0.15f * pow(0.5f + 0.5f * sin(q.y * 3.0f + 0.6f), 0.12f) - 0.15f;
    res.z = hash1(id + 11.0f * floor(0.25f + (q.y * 3.0f + 0.6f) / 6.2831f));
    d *= 0.85f;
    
    {
        vec3 qq = vec3(q.x, abs(q.y - 0.3f) - 5.5f, q.z);
        d = glm::min(d, sdBox3(qq, vec3(1.4f, 0.2f, 1.4f) + sign(q.y - 0.3f) * vec3(0.1f, 0.05f, 0.1f)) - 0.1f);
    }
    
    d = glm::max(d, -sdBox3(p, vec3(14.0f, 10.0f, 6.0f)));
    
    // Floor
    float ra = 0.15f * hash1(id + vec2(1.0f, 3.0f));
    q = p;
    vec2 qxz2 = opRepLim2(vec2(q.x, q.z), 4.0f, vec2(4.0f, 3.0f));
    q.x = qxz2.x;
    q.z = qxz2.y;
    float b = sdBox3(q - vec3(0.0f, -6.0f + 0.1f - ra, 0.0f), vec3(2.0f, 0.5f, 2.0f) - 0.15f - ra) - 0.15f;
    b *= 0.5f;
    if (b < d) { d = b; res.z = hash1(id); }
    
    vec3 pp = p;
    pp.x -= 2.0f;
    pp.z -= 2.0f;
    id = floor((vec2(pp.x, pp.z) + 2.0f) / 4.0f);
    ra = 0.15f * hash1(id + vec2(1.0f, 3.0f) + 23.1f);
    q = pp;
    vec2 qxz3 = opRepLim4(vec2(q.x, q.z), 4.0f, vec2(5.0f, 4.0f), vec2(5.0f, 3.0f));
    q.x = qxz3.x;
    q.z = qxz3.y;
    b = sdBox3(q - vec3(0.0f, -7.0f - ra, 0.0f), vec3(2.0f, 0.6f, 2.0f) - 0.15f - ra) - 0.15f;
    b *= 0.8f;
    if (b < d) { d = b; res.z = hash1(id + 13.5f); }
    pp.x += 2.0f;
    pp.z += 2.0f;
    
    id = floor((vec2(p.x, p.z) + 2.0f) / 4.0f);
    ra = 0.15f * hash1(id + vec2(1.0f, 3.0f) + 37.7f);
    q = p;
    vec2 qxz4 = opRepLim2(vec2(q.x, q.z), 4.0f, vec2(5.0f, 4.0f));
    q.x = qxz4.x;
    q.z = qxz4.y;
    b = sdBox3(q - vec3(0.0f, -8.0f - ra - 1.0f, 0.0f), vec3(2.0f, 0.6f + 1.0f, 2.0f) - 0.15f - ra) - 0.15f;
    b *= 0.5f;
    if (b < d) { d = b; res.z = hash1(id * 7.0f + 31.1f); }
    
    // Roof
    q = vec3(mod(p.x + 2.0f, 4.0f) - 2.0f, p.y, mod(p.z, 4.0f) - 2.0f);
    b = sdBox3(q - vec3(0.0f, 7.0f, 0.0f), vec3(1.95f, 1.0f, 1.95f) - 0.15f) - 0.15f;
    b = glm::max(b, sdBox3(p - vec3(0.0f, 7.0f, 0.0f), vec3(18.0f, 1.0f, 10.0f)));
    if (b < d) { d = b; res.z = hash1(floor((vec2(p.x, p.z) + vec2(2.0f, 0.0f)) / 4.0f) + 31.1f); }
    
    q = vec3(mod(p.x + 0.5f, 1.0f) - 0.5f, p.y, mod(p.z + 0.5f, 1.0f) - 0.5f);
    b = sdBox3(q - vec3(0.0f, 8.0f, 0.0f), vec3(0.45f, 0.5f, 0.45f) - 0.02f) - 0.02f;
    b = glm::max(b, sdBox3(p - vec3(0.0f, 8.0f, 0.0f), vec3(19.0f, 0.2f, 11.0f)));
    if (b < d) { d = b; res.z = hash1(floor((vec2(p.x, p.z) + 0.5f) / 1.0f) + 7.8f); }
    
    b = sdRhombus(vec2(p.y, p.z) - vec2(8.2f, 0.0f), vec2(3.0f, 11.0f), 0.05f);
    q = vec3(mod(p.x + 1.0f, 2.0f) - 1.0f, p.y, mod(p.z + 1.0f, 2.0f) - 1.0f);
    b = glm::max(b, -sdBox3(vec3(abs(p.x) - 20.0f, p.y, q.z) - vec3(0.0f, 8.0f, 0.0f), vec3(2.0f, 5.0f, 0.1f)) - 0.02f);
    b = glm::max(b, -p.y + 8.2f);
    b = glm::max(b, usdBox(p - vec3(0.0f, 8.0f, 0.0f), vec3(19.0f, 12.0f, 11.0f)));
    
    float c = sdRhombus(vec2(p.y, p.z) - vec2(8.3f, 0.0f), vec2(2.25f, 8.5f), 0.05f);
    c = glm::max(c, sdBox1(abs(p.x) - 19.0f, 2.0f));
    b = glm::max(b, -c);
    
    d = glm::min(d, b);
    d = glm::max(d, -sdBox3(p - vec3(0.0f, 9.5f, 0.0f), vec3(15.0f, 4.0f, 9.0f)));
    
    d -= 0.02f * smoothstep(0.5f, 1.0f, fbm3_high(vec3(p.z, p.x, p.y), 0.4f, 2.96f));
    d -= 0.01f * smoothstep(0.4f, 0.8f, fbm3_high(op * 3.0f, 0.4f, 2.96f));
    
    res = vec3(d, 1.0f, res.z);
    
    return res;
}

} // namespace temple_detail

inline float Temple(const vec3& p_in, float /*time*/, uint32_t /*seed*/) {
    vec3 p = p_in;
    p = p * rotationMatrix(vec3(0.0f, 1.0f, 0.0f), -pi / 2.0f);
    const float scale = 0.04f;
    p *= 1.0f / scale;
    return temple_detail::temple(p).x * scale * 0.7f;
}

} // namespace sdf::manufactured

