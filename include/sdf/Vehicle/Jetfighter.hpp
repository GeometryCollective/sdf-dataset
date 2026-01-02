#pragma once
// Ported from Jetfighter.glsl
// Copyright 2017 Kim Berkeby @ingagard - CC BY-NC-SA 3.0

#include "../common.hpp"

namespace sdf::vehicle {

namespace jetfighter_detail {

inline float sgn(float x) { return (x < 0.0f) ? -1.0f : 1.0f; }

inline float sdBox(const vec3& p, const vec3& b) {
    vec3 d = abs(p) - b;
    return glm::min(glm::max(d.x, glm::max(d.y, d.z)), 0.0f) + length(max(d, vec3(0.0f)));
}

inline float sdTorus(const vec3& p, const vec2& t) {
    vec2 q = vec2(length(vec2(p.x, p.z)) - t.x, p.y);
    return length(q) - t.y;
}

inline float sdCapsule(const vec3& p, const vec3& a, const vec3& b, float r) {
    vec3 pa = p - a, ba = b - a;
    float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0f, 1.0f);
    return length(pa - ba * h) - r;
}

inline float sdEllipsoid(const vec3& p, const vec3& r) {
    return (length(p / r) - 1.0f) * r.y;
}

inline float sdConeSection(const vec3& p, float h, float r1, float r2) {
    float d1 = -p.z - h;
    float q = p.z - h;
    float si = 0.5f * (r1 - r2) / h;
    float d2 = glm::max(sqrt(dot(vec2(p.x, p.y), vec2(p.x, p.y)) * (1.0f - si * si)) + q * si - r2, q);
    return length(max(vec2(d1, d2), vec2(0.0f))) + glm::min(glm::max(d1, d2), 0.0f);
}

inline float fCylinder(const vec3& p, float r, float height) {
    float d = length(vec2(p.x, p.y)) - r;
    d = glm::max(d, abs(p.z) - height);
    return d;
}

inline float sdHexPrism(const vec3& p, const vec2& h) {
    vec3 q = abs(p);
    return glm::max(q.y - h.y, glm::max((q.z * 0.866025f + q.x * 0.5f), q.x) - h.x);
}

inline float fOpPipe(float a, float b, float r) {
    return length(vec2(a, b)) - r;
}

inline vec2 pModPolar(vec2 p, float repetitions) {
    float angle = twopi / repetitions;
    float a = atan(p.y, p.x) + angle / 2.0f;
    float r = length(p);
    float c = floor(a / angle);
    a = mod(a, angle) - angle / 2.0f;
    p = vec2(cos(a), sin(a)) * r;
    if (abs(c) >= (repetitions / 2.0f)) c = abs(c);
    return p;
}

inline float pModInterval1(float& p, float size, float start, float stop) {
    float halfsize = size * 0.5f;
    float c = floor((p + halfsize) / size);
    p = mod(p + halfsize, size) - halfsize;
    if (c > stop) {
        p += size * (c - stop);
        c = stop;
    }
    if (c < start) {
        p += size * (c - start);
        c = start;
    }
    return c;
}

inline float pMirror(float& p, float dist) {
    float s = sgn(p);
    p = abs(p) - dist;
    return s;
}

inline mat2 r2(float r) {
    float c = cos(r), s = sin(r);
    return mat2(c, s, -s, c);
}

inline void pR(float& px, float& py, float a) {
    mat2 m = r2(a);
    float nx = m[0][0] * px + m[1][0] * py;
    float ny = m[0][1] * px + m[1][1] * py;
    px = nx; py = ny;
}

inline float fOpUnionRound(float a, float b, float r) {
    vec2 u = max(vec2(r - a, r - b), vec2(0.0f));
    return glm::max(r, glm::min(a, b)) - length(u);
}

inline float fOpIntersectionRound(float a, float b, float r) {
    vec2 u = max(vec2(r + a, r + b), vec2(0.0f));
    return glm::min(-r, glm::max(a, b)) + length(u);
}

inline float MapPlane(const vec3& p_in, float pitch, float roll) {
    vec3 p = p_in;
    float d = 100000.0f;
    float mirrored = 0.0f;
    
    // Mirror position at x=0.0
    mirrored = pMirror(p.x, 0.0f);
    
    float body = glm::min(d, sdEllipsoid(p - vec3(0.0f, 0.1f, -4.40f), vec3(0.50f, 0.30f, 2.0f)));
    body = fOpUnionRound(body, sdEllipsoid(p - vec3(0.0f, 0.0f, 0.50f), vec3(0.50f, 0.40f, 3.25f)), 1.0f);
    body = glm::min(body, sdConeSection(p - vec3(0.0f, 0.0f, 3.8f), 0.1f, 0.15f, 0.06f));
    body = glm::min(body, sdConeSection(p - vec3(0.0f, 0.0f, 3.8f), 0.7f, 0.07f, 0.01f));
    
    // Window
    float winDist = sdEllipsoid(p - vec3(0.0f, 0.3f, -0.10f), vec3(0.45f, 0.4f, 1.45f));
    winDist = fOpUnionRound(winDist, sdEllipsoid(p - vec3(0.0f, 0.3f, 0.60f), vec3(0.3f, 0.6f, 0.75f)), 0.4f);
    winDist = glm::max(winDist, -body);
    body = glm::min(body, winDist) * 0.8f;
    body = glm::min(body, fOpPipe(winDist, sdBox(p - vec3(0.0f, 0.0f, 1.0f), vec3(3.0f, 1.0f, 0.01f)), 0.03f) * 0.7f);
    body = glm::min(body, fOpPipe(winDist, sdBox(p - vec3(0.0f, 0.0f, 0.0f), vec3(3.0f, 1.0f, 0.01f)), 0.03f) * 0.7f);
    
    // Front (nose)
    body = glm::max(body, -glm::max(fCylinder(p - vec3(0.0f, 0.0f, 2.5f), 0.46f, 0.04f), 
                                    -fCylinder(p - vec3(0.0f, 0.0f, 2.5f), 0.35f, 0.1f)));
    vec3 checkPos = p - vec3(0.0f, 0.0f, 2.5f);
    vec2 cpyz = r2(1.57f) * vec2(checkPos.y, checkPos.z);
    checkPos.y = cpyz.x; checkPos.z = cpyz.y;
    body = fOpIntersectionRound(body, -sdTorus(checkPos + vec3(0.0f, 0.80f, 0.0f), vec2(0.6f, 0.05f)), 0.015f);
    body = fOpIntersectionRound(body, -sdTorus(checkPos + vec3(0.0f, 2.30f, 0.0f), vec2(0.62f, 0.06f)), 0.015f);
    
    // Wing simplified
    checkPos = p;
    pR(checkPos.x, checkPos.y, -0.02f);
    float wing = sdBox(checkPos - vec3(4.50f, 0.25f, -4.6f), vec3(3.75f, 0.04f, 2.6f));
    if (wing < 5.0f) {
        checkPos = p - vec3(3.0f, 0.3f, -0.30f);
        pR(checkPos.x, checkPos.z, -0.5f);
        wing = fOpIntersectionRound(wing, -sdBox(checkPos, vec3(6.75f, 1.4f, 2.0f)), 0.1f);
        
        checkPos = p - vec3(8.0f, 0.3f, -8.80f);
        pR(checkPos.x, checkPos.z, -0.05f);
        wing = fOpIntersectionRound(wing, -sdBox(checkPos, vec3(10.75f, 1.4f, 2.0f)), 0.1f);
        
        checkPos = p - vec3(9.5f, 0.3f, -8.50f);
        wing = fOpIntersectionRound(wing, -sdBox(checkPos, vec3(2.0f, 1.4f, 6.75f)), 0.6f);
    }
    d = glm::min(d, wing);
    
    // Rear wing
    float wing2 = sdBox(p - vec3(2.50f, 0.1f, -8.9f), vec3(1.5f, 0.017f, 1.3f));
    if (wing2 < 0.15f) {
        checkPos = p - vec3(3.0f, 0.0f, -5.9f);
        pR(checkPos.x, checkPos.z, -0.5f);
        wing2 = fOpIntersectionRound(wing2, -sdBox(checkPos, vec3(6.75f, 1.4f, 2.0f)), 0.2f);
        
        checkPos = p - vec3(0.0f, 0.0f, -4.9f);
        pR(checkPos.x, checkPos.z, -0.5f);
        wing2 = fOpIntersectionRound(wing2, -sdBox(checkPos, vec3(3.3f, 1.4f, 1.70f)), 0.2f);
    }
    d = glm::min(d, wing2);
    
    // Top wing (vertical stabilizer)
    checkPos = p - vec3(1.15f, 1.04f, -8.5f);
    pR(checkPos.x, checkPos.y, -0.15f);
    float topWing = sdBox(checkPos, vec3(0.014f, 0.8f, 1.2f));
    if (topWing < 0.15f) {
        topWing = glm::min(topWing, sdBox(checkPos - vec3(0.0f, 0.55f, 0.0f), vec3(0.04f, 0.1f, 1.25f)));
    }
    d = glm::min(d, topWing);
    
    // Bottom
    checkPos = p - vec3(0.0f, -0.6f, -5.0f);
    pR(checkPos.y, checkPos.z, 0.07f);
    d = fOpUnionRound(d, sdBox(checkPos, vec3(0.5f, 0.2f, 3.1f)), 0.40f);
    
    // Engine
    d = glm::max(d, -sdBox(p - vec3(0.0f, 0.0f, -9.5f), vec3(1.5f, 0.4f, 0.7f)));
    
    float engineDist = fCylinder(p - vec3(0.40f, -0.1f, -8.7f), 0.42f, 0.2f);
    checkPos = p - vec3(0.4f, -0.1f, -8.3f);
    pR(checkPos.y, checkPos.z, 1.57f);
    engineDist = glm::min(engineDist, sdTorus(checkPos, vec2(0.25f, 0.25f)));
    engineDist = glm::min(engineDist, sdConeSection(p - vec3(0.40f, -0.1f, -9.2f), 0.3f, 0.22f, 0.36f));
    
    d = glm::min(d, engineDist);
    d = glm::min(d, winDist);
    d = glm::min(d, body);
    
    return d;
}

} // namespace jetfighter_detail

inline float Jetfighter(const vec3& p_in, float /*time*/, uint32_t /*seed*/) {
    vec3 p = p_in + vec3(0.0f, 0.0f, 0.8f);
    p = p * rotationMatrix(vec3(0.0f, 1.0f, 0.0f), pi);
    const float scale = 0.12f;
    p *= 1.0f / scale;
    return jetfighter_detail::MapPlane(p, 0.0f, 0.0f) * 0.9f * scale;
}

} // namespace sdf::vehicle

