#pragma once
// Ported from Oldcar.glsl
// Copyright 2019 Florian Berger @flockaroo - CC BY-NC-SA 3.0

#include "../common.hpp"

namespace sdf::vehicle {

namespace oldcar_detail {

inline float sdRoundBox(const vec3& p, const vec3& b, float r) {
    vec3 q = abs(p) - (b - r);
    return length(max(q, vec3(0.0f))) + glm::min(glm::max(q.x, glm::max(q.y, q.z)), 0.0f) - r;
}

inline float sdBox(const vec3& p, const vec3& b) {
    vec3 q = abs(p) - b;
    return length(max(q, vec3(0.0f))) + glm::min(glm::max(q.x, glm::max(q.y, q.z)), 0.0f);
}

inline float sdRoundRect(const vec2& p, const vec2& b, float r) {
    vec2 q = abs(p) - (b - r);
    return length(max(q, vec2(0.0f))) + glm::min(glm::max(q.x, q.y), 0.0f) - r;
}

inline float sdRoundedCylinder(const vec3& p, float R, float r, float h) {
    vec2 d = vec2(length(vec2(p.x, p.z)) - R, abs(p.y) - h * 0.5f);
    return glm::min(glm::max(d.x, d.y), 0.0f) + length(max(d, vec2(0.0f))) - r;
}

inline float sdLine(const vec3& p, const vec3& a, const vec3& b) {
    vec3 pa = p - a, ba = b - a;
    float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0f, 1.0f);
    return length(pa - ba * h);
}

inline float distTorus(const vec3& p, float R, float r) {
    return length(p - vec3(normalize(vec2(p.x, p.y)), 0.0f) * R) - r;
}

inline float sminExp(float a, float b, float k) {
    k = 3.0f / k;
    float res = exp2(-k * a) + exp2(-k * b);
    return -log2(res) / k;
}

inline float fermi(float x) {
    return 1.0f / (1.0f + exp(-x));
}

const float RUMPFW = 1.3f;
const float ALLW = RUMPFW * 1.3f;
const float WheelRadiusF = 0.62f;
const float WheelRadiusR = 0.7f;

inline float distTire(const vec3& p_in, float r) {
    vec3 p = abs(p_in);
    float d = 1000.0f;
    d = glm::min(d, length(p) - r);
    float ang = atan(p.z, p.y);
    float l = length(vec2(p.z, p.y));
    p.x += cos(ang * 100.0f) * 0.005f * smoothstep(0.87f * r, 1.0f * r, l);
    d = glm::max(d, distTorus(vec3(p.y, p.z, p.x) + vec3(0.0f, 0.0f, 0.03f), r * 0.78f, r * 0.28f));
    d = glm::max(d, -l + r * 0.61f);
    float dx = 0.04f;
    float xfr = mod(p.x, dx);
    float x = p.x - xfr + dx * 0.5f;
    d = glm::max(d, -distTorus(vec3(p.y, p.z, p.x) - vec3(0.0f, 0.0f, x), sqrt(r * r - x * x), 0.01f));
    return d;
}

inline float distRim(const vec3& p_in, float r) {
    r *= 0.6f;
    vec3 p0 = p_in;
    vec3 p = p_in;
    p.x = abs(p.x);
    p = vec3(p.z, p.x, p.y);
    float d = 1000.0f;
    d = glm::min(d, sdRoundedCylinder(p, r, 0.01f, 0.6f * r));
    p -= vec3(0.0f, 0.6f * r, 0.0f);
    d = -sminExp(-d, sdRoundedCylinder(p, 0.97f * r, 0.01f, 0.6f * r), 0.015f);
    d = -sminExp(-d, sdRoundedCylinder(p, 0.89f * r, 0.01f, 0.8f * r), 0.015f);
    float d_i = sdRoundedCylinder(p, 0.77f * r, 0.01f, 1.9f * r);
    d = -sminExp(-d, d_i, 0.015f);
    
    float d2 = length(p0 - vec3(-r * 0.6f, 0.0f, 0.0f)) - r * 1.05f;
    d2 = glm::max(d2, d_i);
    d2 = -sminExp(-d2, sdRoundedCylinder(p, 0.4f * r, 0.01f, 0.8f * r), 0.1f);
    d2 = abs(d2) - 0.005f;
    d2 = glm::max(d2, -p0.x);
    
    d = glm::min(d, d2);
    d = glm::min(d, sdRoundedCylinder(p - vec3(0.0f, -0.23f, 0.0f), 0.1f * r, 0.01f, 0.25f));
    d = glm::min(d, sdRoundedCylinder(vec3(p0.z, p0.x, p0.y) - vec3(0.0f, -0.06f, 0.0f), 0.7f * r, 0.02f, 0.05f));
    return d;
}

inline float rille2(float d, float w) {
    return w * exp2(-d * d * 2.0f / w / w);
}

inline float rille(float d, float w) {
    return w * exp2(-abs(d) * 1.44f / w);
}

inline vec2 distCar(const vec3& p_in) {
    vec3 p = p_in;
    
    // Rotation for slight tilt
    float co = cos(-0.023f);
    float si = sin(-0.023f);
    float py = p.y * co - p.z * si;
    float pz = p.y * si + p.z * co;
    p.y = py; p.z = pz;
    
    float d = 1000.0f;
    float mat = -1.0f;
    
    p *= 2.0f;
    if (p.x < 0.0f) p.x = -p.x;
    vec3 p0 = p + vec3(0.0f, 0.0f, 0.13f);
    
    // Main body (rumpf)
    p = p0 + vec3(0.0f, 0.1f, 0.0f);
    float drumpf = sdRoundBox(p, vec3(RUMPFW + p.y * 0.15f - p.y * p.y * 0.04f + p.z * p.y * 0.03f,
                                      3.2f - p.z * 0.3f + p.z * p.z * 0.1f - step(0.0f, -p.y) * p.x * 0.3f - step(0.0f, p.y) * 0.4f * p.z,
                                      0.8f + p.y * 0.02f - p.x * p.x * 0.05f * (1.0f + 0.01f * (p.y * p.y * p.y * p.y))),
                              glm::max(p.y * 0.04f, mix(0.25f + p.y * 0.05f, 0.07f, -p.z * 1.5f + 0.5f))) * 0.7f;
    
    // Cabin
    p = p0 - vec3(0.0f, 0.5f, 0.7f);
    float dcabin = sdRoundBox(p, vec3(RUMPFW * 1.04f + p.y * 0.07f - p.y * p.y * 0.08f + p.z * 0.0f,
                                      1.2f - p.z * (0.7f - 0.4f * step(0.0f, p.y)),
                                      0.7f + p.y * 0.07f - p.x * p.x * 0.05f - p.y * p.y * 0.05f),
                              0.33f + 0.15f * p.y) * 0.7f;
    d = glm::min(d, dcabin);
    
    // Front screen
    p = p0 - vec3(0.0f, 0.4f, 0.88f + 0.10f - 0.06f * p.x * p.x);
    float sy = step(0.0f, p.y);
    float dfrontscr = sdRoundRect(vec2(p.x, p.z) - vec2(0.0f, -0.12f + sy * 0.1f), 
                                  vec2(RUMPFW * 0.4f - sy * 0.2f, 0.12f - step(0.0f, p.y) * 0.07f) * 2.0f, 
                                  0.14f - sy * 0.04f) * 0.7f;
    dfrontscr = glm::max(dfrontscr, -(drumpf - 0.07f));
    d = -sminExp(-d, dfrontscr, 0.03f);
    
    // Side screens
    p = p0 - vec3(0.0f, 0.23f, 0.91f);
    vec3 sidebox = vec3(2.0f, 0.35f - (p.z + 0.3f) * (0.1f + step(0.0f, p.z + 0.3f) * 0.1f), 
                        0.065f + p.y * 0.023f - 0.05f * p.y * p.y * step(0.0f, p.z)) * 2.0f;
    float dsidescr = sdRoundRect(vec2(p.y, p.z) - vec2(0.1f * p.z, 0.015f * p.y), vec2(sidebox.y, sidebox.z), 0.1f + 0.07f * p.y) * 0.7f;
    d = -sminExp(-d, dsidescr, 0.05f);
    d -= clamp((abs(dsidescr - 0.03f) - 0.016f) * 0.2f, -0.02f, 0.0f);
    
    // Floor line
    p = p0 + vec3(0.0f, 0.1f, 0.0f);
    float dfloorline = p0.z + 0.7f - 0.5f * (0.5f - 0.5f * cos((p0.y * 1.0f - p0.y * p0.y * 0.1f) * step(0.0f, -p0.y)));
    
    drumpf -= glm::max(-dfloorline * 0.5f - 1.3f * dfloorline * dfloorline, 0.0f) * 5.0f * 
              clamp(((-p0.y - 1.8f) - 0.9f * (-p0.y - 1.8f) * (-p0.y - 1.8f)) * abs(-p0.y - 1.8f), 0.0f, 11.0f);
    
    // Side stripe
    p = p0 + vec3(0.0f, 0.1f, 0.0f);
    drumpf -= 0.6f * rille2(p.z - 0.4f + 0.03f * p.y - 0.1f * p.x, 0.02f) * (1.0f - smoothstep(2.4f, 2.6f, abs(p0.y + 0.3f)));
    
    d = sminExp(d, drumpf, 0.03f);
    
    // Grill hole
    float z = p.z + 0.2f;
    float dgrillhole = sdRoundBox(p - vec3(0.0f, -3.0f, -0.02f), 
                                  vec3(0.18f * 0.9f * RUMPFW - step(0.0f, -z) * z * z * 0.58f * RUMPFW, 0.5f, 0.33f) * 2.0f, 0.1f);
    d = -sminExp(-d, dgrillhole, 0.04f);
    float f = smoothstep(-3.0f, 0.0f, p.y);
    d = -sminExp(-d, -dgrillhole + 0.1f + f * 10.0f, 0.04f);
    mat = 1.0f; // CARBODY
    
    // Grill lines
    p = p0 - vec3(0.0f, -3.26f + 0.3f * p.z + 0.35f * p.x - 0.1f * p.z * p.z, 0.0f);
    p.x = mod(p.x + 0.005f, 0.025f) - 0.0125f;
    d = glm::min(d, glm::max(dgrillhole, (length(vec2(p.x, p.y)) - 0.007f) * 0.8f));
    
    // Window glass cutout
    d = glm::max(d, -dcabin - 0.06f);
    
    // Window glass
    d = glm::min(d, dcabin + 0.035f);
    
    p0 -= vec3(0.0f, 0.0f, 0.13f);
    
    // Wheels
    const vec3 PF = vec3(ALLW * 0.78f * 0.5f, -1.43f, -0.33f) * 2.0f;
    const vec3 PR = vec3(ALLW * 0.96f * 0.5f, 1.23f, -0.35f) * 2.0f;
    
    vec3 pf = p0 - vec3(PF.x, PF.y, PF.z);
    vec3 pr = p0 - vec3(PR.x, PR.y, PR.z);
    
    float rear = step(0.0f, p_in.y);
    p = mix(pf, pr, rear);
    float siz = mix(WheelRadiusF, WheelRadiusR, rear);
    
    d = glm::min(d, distTire(p, siz));
    d = glm::min(d, distRim(p, siz));
    
    return vec2(d * 0.5f, mat);
}

} // namespace oldcar_detail

inline float Oldcar(const vec3& p_in, float /*time*/, uint32_t /*seed*/) {
    vec3 p = p_in + vec3(0.0f, 0.05f, 0.0f);
    p = p * rotationMatrix(vec3(1.0f, 0.0f, 0.0f), pi / 2.0f);
    const float scale = 0.5f;
    p *= 1.0f / scale;
    return oldcar_detail::distCar(p).x * scale;
}

} // namespace sdf::vehicle

