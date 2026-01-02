#pragma once
// Ported from Jellyfish.glsl
// Copyright 2017 Martijn Steinrucken @BigWings - CC BY-NC-SA 3.0

#include "../common.hpp"

namespace sdf::animal {

namespace jellyfish_detail {

inline float N1(float x) { return fract(sin(x) * 5346.1764f); }
inline float N2(float x, float y) { return N1(x + y * 23414.324f); }

inline float N3(const vec3& p_in) {
    vec3 p = fract(p_in * 0.3183099f + 0.1f);
    p *= 17.0f;
    return fract(p.x * p.y * p.z * (p.x + p.y + p.z));
}

inline vec3 N31(float p) {
    vec3 p3 = fract(vec3(p) * vec3(0.1031f, 0.11369f, 0.13787f));
    p3 += dot(p3, vec3(p3.y, p3.z, p3.x) + 19.19f);
    return fract(vec3((p3.x + p3.y) * p3.z, (p3.x + p3.z) * p3.y, (p3.y + p3.z) * p3.x));
}

inline float jelly_smin(float a, float b, float k) {
    float h = clamp(0.5f + 0.5f * (b - a) / k, 0.0f, 1.0f);
    return mix(b, a, h) - k * h * (1.0f - h);
}

inline float jelly_smax(float a, float b, float k) {
    float h = clamp(0.5f + 0.5f * (b - a) / k, 0.0f, 1.0f);
    return mix(a, b, h) + k * h * (1.0f - h);
}

inline float sdSphere(const vec3& p, const vec3& pos, float s) {
    return length(p - pos) - s;
}

inline vec2 pModPolar(vec2& p, float repetitions, float fix) {
    float angle = pi * 2.0f / repetitions;
    float a = atan(p.y, p.x) + angle / 2.0f;
    float r = length(p);
    float c = floor(a / angle);
    a = mod(a, angle) - (angle / 2.0f) * fix;
    p = vec2(cos(a), sin(a)) * r;
    return p;
}

inline float remap(float a, float b, float c, float d, float t) {
    return ((t - a) / (b - a)) * (d - c) + c;
}

inline float jelly_map(const vec3& p_in, const vec3& id, float time) {
    float t = time * 2.0f;
    
    float N = N3(id);
    
    float x = (p_in.y + N * pi * 2.0f) * 1.0f + t;
    float r = 1.0f;
    
    float pump = cos(x + cos(x)) + sin(2.0f * x) * 0.2f + sin(4.0f * x) * 0.02f;
    
    x = t + N * pi * 2.0f;
    vec3 p = p_in;
    p.y -= (cos(x + cos(x)) + sin(2.0f * x) * 0.2f) * 0.6f;
    p.x *= 1.0f + pump * 0.2f;
    p.z *= 1.0f + pump * 0.2f;
    
    float d1 = sdSphere(p, vec3(0.0f, 0.0f, 0.0f), r);
    float d2 = sdSphere(p, vec3(0.0f, -0.5f, 0.0f), r);
    
    float d = jelly_smax(d1, -d2, 0.1f);
    
    if (p.y < 0.5f) {
        float sway = sin(t + p.y + N * pi * 2.0f) * smoothstep(0.5f, -3.0f, p.y) * N * 0.3f;
        vec3 mp = p;
        mp.x += sway * N;
        mp.z += sway * (1.0f - N);
        
        vec2 mpxz = vec2(mp.x, mp.z);
        pModPolar(mpxz, 6.0f, 0.0f);
        mp.x = mpxz.x;
        mp.z = mpxz.y;
        
        float d3 = length(vec2(mp.x, mp.z) - vec2(0.2f, 0.1f)) - remap(0.5f, -3.5f, 0.1f, 0.01f, mp.y);
        d3 += (sin(mp.y * 10.0f) + sin(mp.y * 23.0f)) * 0.03f;
        
        float d32 = length(vec2(mp.x, mp.z) - vec2(0.2f, 0.1f)) - remap(0.5f, -3.5f, 0.1f, 0.04f, mp.y) * 0.5f;
        d3 = glm::min(d3, d32);
        d = jelly_smin(d, d3, 0.5f);
        
        if (p.y < 0.2f) {
            vec3 op = p;
            op.x += sway * N;
            op.z += sway * (1.0f - N);
            vec2 opxz = vec2(op.x, op.z);
            pModPolar(opxz, 13.0f, 1.0f);
            op.x = opxz.x;
            op.z = opxz.y;
            
            float d4 = length(vec2(op.x, op.z) - vec2(0.85f, 0.0f)) - remap(0.5f, -3.0f, 0.04f, 0.0f, op.y);
            d = jelly_smin(d, d4, 0.15f);
        }
    }
    
    d *= 0.8f;
    return d;
}

} // namespace jellyfish_detail

inline float Jellyfish(const vec3& p_in, float time, uint32_t /*seed*/) {
    vec3 p = p_in + vec3(0.0f, -0.6f, 0.0f);
    const float scale = 0.26f;
    p *= 1.0f / scale;
    return jellyfish_detail::jelly_map(p, vec3(0.0f), time) * scale;
}

} // namespace sdf::animal

