#pragma once
// Ported from MantaRay.glsl
// Copyright 2015 @dakrunch - CC BY-NC-SA 3.0

#include "../common.hpp"

namespace sdf::animal {

namespace mantaray_detail {

inline float softMin(float a, float b, float k) {
    float h = clamp(0.5f + 0.5f * (b - a) / k, 0.0f, 1.0f);
    return mix(b, a, h) - k * h * (1.0f - h);
}

inline float sphere(const vec3& p, float r) {
    return length(p) - r;
}

inline float sdEllipsoid(const vec3& p, const vec3& r) {
    float smallestSize = glm::min(glm::min(r.x, r.y), r.z);
    vec3 deformedP = p / r;
    float d = length(deformedP) - 1.0f;
    return d * smallestSize;
}

inline float sdBox(const vec3& p, const vec3& b) {
    vec3 d = abs(p) - b;
    return glm::min(glm::max(d.x, glm::max(d.y, d.z)), 0.0f) + length(max(d, vec3(0.0f)));
}

inline float wings(const vec3& p) {
    vec3 r = vec3(1.5f, 0.15f, 0.55f);
    float smallestSize = glm::min(glm::min(r.x, r.y), r.z);
    
    vec3 dp = p / r;
    dp.z -= dp.x * dp.x * 0.8f;
    dp.z -= (dp.x - 0.6f) * (dp.x - 0.5f);
    dp.y -= 0.6f;
    
    float d = (dp.y * dp.y + dp.z * dp.z);
    d += abs(dp.x);
    d -= 1.0f;
    
    return d * smallestSize;
}

inline float mantabody(const vec3& p_in) {
    vec3 p = p_in;
    
    float d = sdEllipsoid(p, vec3(0.4f, 0.3f, 0.8f));
    
    if (p.z < 1.0f && p.z > -1.4f && p.y < 1.0f && p.y > -0.2f) {
        d = softMin(d, wings(p), 0.4f);
    }
    
    vec3 flapsP;
    vec3 flapsScale;
    
    if (p.x < 1.0f && p.z < -0.2f && p.z > -1.4f && p.y < 0.2f && p.y > -0.2f) {
        flapsP = p;
        flapsP += vec3(-0.5f - p.z * 0.2f, 0.3f - p.x * 0.5f, 1.0f - p.x * 0.2f);
        flapsScale = vec3(0.09f, 0.08f, 0.25f);
        d = softMin(d, sdEllipsoid(flapsP, flapsScale), 0.2f);
    }
    
    if (p.x < 0.2f && p.z > 0.3f && p.z < 1.0f && p.y > 0.1f && p.y < 0.5f) {
        flapsP = p;
        flapsP += vec3(0.0f, -0.15f - 0.2f * p.z, -0.7f);
        flapsScale = vec3(0.03f, 0.1f, 0.2f);
        d = softMin(d, sdEllipsoid(flapsP, flapsScale), 0.15f);
    }
    
    float taild = glm::max(length(vec2(p.x, p.y)), -sdBox(p + vec3(0.0f, 0.0f, 1.0f), vec3(1.0f)));
    d = softMin(d, taild, 0.1f);
    
    return d;
}

inline float animatedManta(const vec3& p_in, float time) {
    float size = 1.0f;
    
    float timeloop = time * 2.5f / (size - 0.25f);
    vec3 p = p_in;
    p.y += -sin(timeloop - 0.5f) * 0.25f * size;
    p.y += sin(time * 0.5f) * 0.1f;
    
    vec3 mantap = p / size;
    mantap.x = abs(mantap.x);
    
    float animation = sin(timeloop - 3.0f - 1.3f * mantap.z);
    float animationAmount = pow(mantap.x, 1.5f);
    animationAmount = glm::min(animationAmount, 2.5f);
    mantap.y += animation * (0.3f * animationAmount + 0.15f);
    
    float d = mantabody(mantap);
    
    return d * size;
}

} // namespace mantaray_detail

inline float MantaRay(const vec3& p_in, float time, uint32_t /*seed*/) {
    vec3 p = p_in;
    p = p * rotationMatrix(vec3(0.0f, 1.0f, 0.0f), pi);
    const float scale = 0.5f;
    p *= 1.0f / scale;
    return mantaray_detail::animatedManta(p, time) * 0.3f * scale;
}

} // namespace sdf::animal

