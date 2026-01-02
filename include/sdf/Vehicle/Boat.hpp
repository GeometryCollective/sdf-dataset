#pragma once
// Ported from Boat.glsl
// Copyright 2017 @dr2 - CC BY-NC-SA 3.0

#include "../common.hpp"

namespace sdf::vehicle {

namespace boat_detail {

inline float PrBoxDf(const vec3& p, const vec3& b) {
    vec3 d = abs(p) - b;
    return glm::min(glm::max(d.x, glm::max(d.y, d.z)), 0.0f) + length(max(d, vec3(0.0f)));
}

inline float PrBox2Df(const vec2& p, const vec2& b) {
    vec2 d = abs(p) - b;
    return glm::min(glm::max(d.x, d.y), 0.0f) + length(max(d, vec2(0.0f)));
}

inline float PrCylDf(const vec3& p, float r, float h) {
    return glm::max(length(vec2(p.x, p.y)) - r, abs(p.z) - h);
}

inline float PrCapsDf(const vec3& p, float r, float h) {
    return length(p - vec3(0.0f, 0.0f, h * clamp(p.z / h, -1.0f, 1.0f))) - r;
}

inline float PrEECapsDf(const vec3& p, const vec3& v1, const vec3& v2, float r) {
    vec3 s = p - v1;
    vec3 t = v2 - v1;
    return length(s - clamp(dot(s, t) / dot(t, t), 0.0f, 1.0f) * t) - r;
}

inline vec2 Rot2D(const vec2& q, float a) {
    return q * cos(a) + vec2(q.y, q.x) * sin(a) * vec2(-1.0f, 1.0f);
}

inline vec2 Rot2Cs(const vec2& q, const vec2& cs) {
    return vec2(dot(q, vec2(cs.x, -cs.y)), dot(vec2(q.y, q.x), cs));
}

struct ShipState {
    vec3 qHit;
    vec3 bDeck;
    float szFac;
    float dstFar;
};

inline float ShipDf(vec3 p, ShipState& state) {
    state.szFac = 0.6f;
    state.dstFar = 100.0f;
    
    float rRig = 0.02f;
    float rSpar = 0.05f;
    
    // Ship configuration - static ship
    vec3 shipConf = vec3(0.0f, 0.0f, 0.0f);
    
    p.y -= shipConf.x + 4.1f * state.szFac;
    p /= state.szFac;
    float dMin = state.dstFar / state.szFac;
    float fy = 1.0f - 0.07f * p.y;
    float fz = 1.0f - 0.14f * step(1.0f, abs(p.z));
    float zLim = abs(p.z) - 4.5f;
    vec3 q = p;
    float d = zLim;
    q.z = mod(q.z + 1.4f, 2.8f) - 1.2f;
    d = glm::max(d, PrCapsDf(vec3(q.x, q.z, q.y) - vec3(0.0f, 0.0f, 3.7f * (fz - 1.0f)), 0.1f * fy, 3.7f * fz));
    state.qHit = q;
    
    q = p;
    float yLim = abs(q.y - 0.2f * fz) - 3.0f * fz;
    vec3 qq = q;
    qq.y = mod(qq.y - 3.3f * (fz - 1.0f), 2.0f * fz) - fz;
    qq.z = mod(qq.z + 1.4f, 2.8f) - 1.4f + 0.1f * fz;
    d = glm::max(glm::max(glm::min(d, PrCylDf(vec3(qq.x, qq.z, qq.y) - vec3(0.0f, 0.1f * fz - 0.23f, 0.05f * fy * fz),
       0.15f * fy, 0.11f * fy * fz)), yLim), zLim);
    
    d = glm::max(glm::max(PrCapsDf(vec3(qq.y, qq.z, qq.x), 0.05f, 1.23f * fy * fz), yLim), zLim);
    
    q = p;
    d = glm::min(d, glm::min(PrEECapsDf(q, vec3(0.0f, -3.5f, 4.3f), vec3(0.0f, -2.6f, 6.7f), rSpar),
       PrEECapsDf(q, vec3(0.0f, -4.0f, 4.1f), vec3(0.0f, -2.9f, 6.0f), rSpar)));
    d = glm::min(d, glm::min(PrEECapsDf(q, vec3(0.0f, -1.2f, -3.0f), vec3(0.0f, -0.5f, -4.5f), rSpar),
       PrEECapsDf(q, vec3(0.0f, -2.7f, -3.0f), vec3(0.0f, -2.7f, -4.5f), rSpar)));
    
    q = p;
    qq = q;
    qq.y = mod(qq.y - 3.1f * (fz - 1.0f), 2.0f * fz) - fz;
    qq.z = mod(qq.z + 1.4f, 2.8f) - 1.4f + 0.2f * (fz - abs(qq.y)) * (fz - abs(qq.y)) - 0.1f * fz;
    d = glm::max(glm::max(glm::max(PrBoxDf(qq, vec3((1.2f - 0.07f * q.y) * fz, fz, 0.01f)),
       glm::min(qq.y, 1.5f * fy * fz - length(vec2(qq.x, qq.y + 0.9f * fy * fz)))),
       abs(q.y - 3.0f * (fz - 1.0f)) - 2.95f * fz), -PrBox2Df(vec2(qq.y, qq.z), vec2(0.01f * fz)));
    d = glm::max(d, zLim);
    
    q = p;
    q.z -= -3.8f;
    q.y -= -1.75f - 0.2f * q.z;
    d = glm::min(d, PrBoxDf(q, vec3(0.01f, 0.9f - 0.2f * q.z, 0.6f)));
    
    q = p;
    q.y -= -1.0f;
    q.z -= 4.5f;
    vec3 w = vec3(1.0f, q.y, q.z);
    d = glm::min(d, glm::max(glm::max(glm::max(abs(q.x) - 0.01f, -dot(w, vec3(2.3f, 1.0f, -0.35f))),
       -dot(w, vec3(0.68f, -0.74f, -1.0f))), -dot(w, vec3(0.41f, 0.4f, 1.0f))));
    
    q = p;
    float gz = (q.z - 0.5f) / 5.0f + 0.3f;
    gz *= gz;
    gz = 1.05f * (1.0f - 0.45f * gz * gz);
    q.x = abs(q.x);
    q.z = mod(q.z + 1.4f, 2.8f) - 1.4f;
    d = glm::max(zLim, glm::min(PrEECapsDf(q, vec3(1.05f * gz, -3.25f, -0.5f), vec3(1.4f * fz, -2.95f, -0.05f), 0.7f * rRig),
       PrEECapsDf(vec3(q.x, q.y, abs(q.z + 0.2f) - 0.01f * (0.3f - 2.0f * q.y)), vec3(gz, -3.2f, 0.0f),
       vec3(0.05f, -0.9f + 2.0f * (fz - 1.0f), 0.0f), rRig)));
    
    q = p;
    d = glm::min(d, PrEECapsDf(q, vec3(0.0f, -3.0f, -4.45f), vec3(0.0f, -2.7f, -4.5f), 0.8f * rRig));
    d = glm::min(glm::min(d, glm::min(PrEECapsDf(q, vec3(0.0f, 2.45f, 2.65f), vec3(0.0f, -2.7f, 6.5f), rRig),
       PrEECapsDf(q, vec3(0.0f, 2.5f, 2.65f), vec3(0.0f, -3.2f, 4.9f), rRig))),
       PrEECapsDf(q, vec3(0.0f, 2.6f, -3.0f), vec3(0.0f, -0.5f, -4.5f), rRig));
    q.x = abs(q.x);
    d = glm::min(d, PrEECapsDf(q, vec3(0.65f, -3.5f, 3.5f), vec3(0.05f, -2.7f, 6.4f), rRig));
    
    float s = step(1.8f, q.y) - step(q.y, -0.2f);
    d = glm::min(glm::min(d, glm::min(PrEECapsDf(q, vec3(0.95f, 0.4f, 2.7f) + vec3(-0.1f, 1.7f, 0.0f) * s,
       vec3(0.05f, 1.1f, -0.15f) + vec3(0.0f, 2.0f, 0.0f) * s, rRig),
       PrEECapsDf(q, vec3(1.05f, 1.0f, -0.1f) + vec3(-0.1f, 2.0f, 0.0f) * s,
       vec3(0.05f, 0.5f, -2.95f) + vec3(0.0f, 1.7f, 0.0f) * s, rRig))),
       PrEECapsDf(q, vec3(0.95f, 0.4f, -2.9f) + vec3(-0.1f, 1.7f, 0.0f) * s,
       vec3(0.05f, 0.9f, -0.25f) + vec3(0.0f, 2.0f, 0.0f) * s, rRig));
    
    q = p;
    q.y -= 3.4f;
    q.z -= 0.18f;
    d = glm::min(d, PrBoxDf(q, vec3(0.01f, 0.2f, 0.3f)));
    
    q = p;
    q.z = mod(q.z + 1.4f, 2.8f) - 1.4f;
    q.y -= -3.4f;
    q.z -= -0.4f;
    d = glm::max(zLim, PrBoxDf(q, vec3(0.3f, 0.1f, 0.5f)));
    
    q = p;
    q.x = abs(q.x);
    q.y -= -3.8f;
    q.z -= 0.5f;
    fz = q.z / 5.0f + 0.3f;
    fz *= fz;
    fy = 1.0f - smoothstep(-1.3f, -0.1f, q.y);
    gz = smoothstep(2.0f, 5.0f, q.z);
    state.bDeck = vec3((1.0f - 0.45f * fz * fz) * (1.1f - 0.5f * fy * fy) *
       (1.0f - 0.5f * smoothstep(-5.0f, -2.0f, q.y) * smoothstep(2.0f, 5.0f, q.z)),
       0.78f - 0.8f * gz * gz - 0.2f * (1.0f - smoothstep(-5.2f, -4.0f, q.z)), 5.0f);
    d = glm::min(d, glm::min(PrBoxDf(vec3(q.x, q.y + state.bDeck.y - 0.6f, q.z), state.bDeck),
       glm::max(PrBoxDf(q - vec3(0.0f, 0.72f, -4.6f), vec3(state.bDeck.x, 0.12f, 0.4f)),
       -PrBox2Df(vec2(abs(q.x) - 0.4f, q.y - 0.65f), vec2(0.2f, 0.08f)))));
    d = glm::max(d, -PrBoxDf(vec3(q.x, q.y - 0.58f - 0.1f * fz, q.z), vec3(state.bDeck.x - 0.07f, 0.3f, state.bDeck.z - 0.1f)));
    
    q = p;
    d = glm::max(d, -glm::max(PrBox2Df(vec2(q.y + 3.35f, mod(q.z + 0.25f, 0.5f) - 0.25f), vec2(0.08f, 0.1f)),
       abs(q.z + 0.5f) - 3.75f));
    dMin = glm::min(dMin, d);
    
    q = p;
    d = PrBoxDf(q + vec3(0.0f, 4.4f, 4.05f), vec3(0.03f, 0.35f, 0.5f));
    dMin = glm::min(dMin, d);
    
    return 0.7f * dMin * state.szFac;
}

} // namespace boat_detail

inline float Boat(const vec3& p_in, float /*time*/, uint32_t /*seed*/) {
    vec3 p = p_in + vec3(0.0f, 0.5f, 0.1f);
    const float scale = 0.25f;
    p *= (1.0f / scale);
    boat_detail::ShipState state;
    return boat_detail::ShipDf(p, state) * scale;
}

} // namespace sdf::vehicle

