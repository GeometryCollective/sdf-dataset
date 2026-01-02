#pragma once
// Ported from Snake.glsl
// Copyright 2020 Martijn Steinrucken @BigWings - CC BY-NC-SA 3.0

#include "../common.hpp"

namespace sdf::animal {

namespace snake_detail {

const float MAX_DIST = 60.0f;

inline mat2 Rot(float a) {
    float s = sin(a);
    float c = cos(a);
    return mat2(c, -s, s, c);
}

inline vec2 Hash22(vec2 p) {
    vec3 p3 = fract(vec3(p.x, p.y, p.x) * vec3(0.1031f, 0.1030f, 0.0973f));
    p3 += dot(p3, vec3(p3.y, p3.z, p3.x) + 33.33f);
    return fract((vec2(p3.x, p3.x) + vec2(p3.y, p3.z)) * vec2(p3.z, p3.y));
}

inline float sabs(float x, float k) {
    float a = (0.5f / k) * x * x + k * 0.5f;
    float b = abs(x);
    return b < k ? a : b;
}

inline float smin_snake(float a, float b, float k) {
    float h = clamp(0.5f + 0.5f * (b - a) / k, 0.0f, 1.0f);
    return mix(b, a, h) - k * h * (1.0f - h);
}

inline float smax_snake(float a, float b, float k) {
    return smin_snake(a, b, -k);
}

inline float sdSph(const vec3& p, const vec3& pos, const vec3& squash, float r) {
    vec3 sq = 1.0f / squash;
    vec3 pp = (p - pos) * sq;
    return (length(pp) - r) / glm::max(sq.x, glm::max(sq.y, sq.z));
}

inline float sdGyroid(const vec3& p_in, float scale, float thickness, float bias) {
    vec3 p = p_in * scale;
    return abs(dot(sin(p), cos(vec3(p.z, p.x, p.y))) + bias) / scale - thickness;
}

inline vec3 sdBody(const vec3& p_in, float time) {
    float t = time * 0.3f;
    float neckFade = smoothstep(3.0f, 10.0f, p_in.z);
    
    vec3 p = p_in;
    p.x += sin(p.z * 0.15f - t) * neckFade * 4.0f;
    p.y += sin(p.z * 0.1f - t) * neckFade;
    
    float body = length(vec2(p.x, p.y)) - (0.86f + smoothstep(2.0f, 15.0f, p.z) * 0.6f - p.z * 0.01f);
    body = glm::max(0.8f - p.z, body);
    
    // Simplified scale texture
    float scales = 0.0f;
    if (body < 0.1f) {
        vec2 st = vec2(atan(p.x, p.y), p.z);
        vec2 uv = vec2(-st.y * 0.25f, st.x / 6.2832f + 0.5f);
        float a = sin(st.x + 1.57f) * 0.5f + 0.5f;
        float fade = a;
        a = smoothstep(0.1f, 0.4f, a);
        
        uv.y = 1.0f - abs(uv.y * 2.0f - 1.0f);
        uv.y *= (uv.y - 0.2f) * 0.4f;
        
        // Simplified scale pattern
        float scalePattern = sin(uv.x * 50.0f) * sin(uv.y * 50.0f);
        scales = scalePattern * 0.02f * (fade + 0.2f);
    }
    
    body += scales;
    body += smoothstep(-0.4f, -0.9f, p.y) * 0.2f;
    
    return vec3(body, 0.0f, 0.0f);
}

inline float GetHeadScales(const vec3& p_in, float md) {
    float jitter = 0.5f;
    jitter *= smoothstep(0.1f, 0.3f, abs(md));
    jitter *= smoothstep(1.2f, 0.5f, p_in.z);
    
    vec3 p = p_in;
    p.z += 0.5f;
    p.z *= 0.5f;
    
    // Simplified rotation
    float co = cos(0.6f);
    float si = sin(0.6f);
    float py_new = p.y * co - p.z * si;
    float pz_new = p.y * si + p.z * co;
    p.y = py_new;
    p.z = pz_new;
    
    float y_angle = atan(p.y, p.x);
    vec2 gv = vec2(p.z * 5.0f, y_angle * 3.0f);
    vec2 id = floor(gv);
    gv = fract(gv) - 0.5f;
    
    float d = MAX_DIST;
    for (float yy = -1.0f; yy <= 1.0f; yy++) {
        for (float xx = -1.0f; xx <= 1.0f; xx++) {
            vec2 offs = vec2(xx, yy);
            vec2 n = Hash22(id + offs);
            vec2 pp = offs + sin(n * 6.2831f) * jitter;
            pp -= gv;
            float cd = dot(pp, pp);
            if (cd < d) d = cd;
        }
    }
    
    d += sin(d * 20.0f) * 0.02f;
    d *= smoothstep(0.0f, 0.5f, length(vec2(p.x, p.y)) - 0.1f);
    return d * 0.06f;
}

inline float sdHead(const vec3& p_in) {
    vec3 p = p_in;
    p.x = abs(p.x * 0.9f);
    float d = sdSph(p, vec3(0.0f, -0.05f, 0.154f), vec3(1.0f, 1.0f, 1.986f), 1.14f);
    d = smax_snake(d, length(p - vec3(0.0f, 7.89f, 0.38f)) - 8.7f, 0.2f);
    d = smax_snake(d, length(p - vec3(0.0f, -7.71f, 1.37f)) - 8.7f, 0.15f);
    
    d = smax_snake(d, 8.85f - length(p - vec3(9.16f, -1.0f, -3.51f)), 0.2f);
    
    vec3 ep = p - vec3(0.54f, 0.265f, -0.82f);
    float eye = length(ep) - 0.35f;
    float brows = smoothstep(0.1f, 0.8f, p.y - (p.z + 0.9f) * 0.5f);
    brows *= brows * brows;
    brows *= smoothstep(0.3f, -0.2f, eye);
    d -= brows * 0.5f;
    d += smoothstep(0.1f, -0.2f, eye) * 0.1f;
    
    vec2 mp = vec2(p.y, p.z) - vec2(3.76f + smoothstep(-0.71f, -0.14f, p.z) * (p.z + 0.5f) * 0.2f, -0.71f);
    float mouth = length(mp) - 4.24f;
    d += smoothstep(0.03f, 0.0f, abs(mouth)) * smoothstep(0.59f, 0.0f, p.z) * 0.03f;
    
    d += GetHeadScales(p, mouth);
    
    d = glm::min(d, eye);
    
    float nostril = length(vec2(p.z, p.y) - vec2(-1.9f - p.x * p.x, 0.15f)) - 0.05f;
    d = smax_snake(d, -nostril, 0.05f);
    return d;
}

inline float sdTongue(const vec3& p_in, float time) {
    float t = time * 3.0f;
    
    float inOut = smoothstep(0.7f, 0.8f, sin(t * 0.5f));
    
    if (p_in.z > -2.0f || inOut == 0.0f) return MAX_DIST;
    
    float zigzag = (abs(fract(t * 2.0f) - 0.5f) - 0.25f) * 4.0f;
    float tl = 2.5f;
    
    vec3 p = p_in + vec3(0.0f, 0.27f, 2.0f);
    p.z *= -1.0f;
    float z = p.z;
    
    // yz rotation
    float co = cos(z * 0.4f * zigzag);
    float si = sin(z * 0.4f * zigzag);
    float py_new = p.y * co - p.z * si;
    float pz_new = p.y * si + p.z * co;
    p.y = py_new;
    p.z = pz_new;
    
    p.z -= inOut * tl;
    
    float width = smoothstep(0.0f, -1.0f, p.z);
    float fork = 1.0f - width;
    
    float r = mix(0.05f, 0.02f, fork);
    
    p.x = sabs(p.x, 0.05f * width * width);
    p.x -= r + 0.01f;
    p.x -= fork * 0.2f * inOut;
    
    return length(p - vec3(0.0f, 0.0f, clamp(p.z, -tl, 0.0f))) - r;
}

inline float GetDist(const vec3& P, float time) {
    vec3 p = P;
    float co = cos(sin(time * 0.3f) * 0.1f * smoothstep(1.0f, 0.0f, p.z));
    float si = sin(sin(time * 0.3f) * 0.1f * smoothstep(1.0f, 0.0f, p.z));
    float px_new = p.x * co - p.z * si;
    float pz_new = p.x * si + p.z * co;
    p.x = px_new;
    p.z = pz_new;
    
    float d = sdTongue(p, time) * 0.7f;
    d = glm::min(d, sdHead(p));
    d = smin_snake(d, sdBody(P, time).x, 0.13f);
    
    return d;
}

} // namespace snake_detail

inline float Snake(const vec3& p_in, float time, uint32_t /*seed*/) {
    vec3 p = p_in;
    p = p * rotationMatrix(vec3(0.0f, 1.0f, 0.0f), pi);
    float scale = 0.25f;
    p *= 1.0f / scale;
    return glm::min(1.0f, snake_detail::GetDist(p, time) * scale);
}

} // namespace sdf::animal

