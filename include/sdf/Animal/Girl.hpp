#pragma once
// Ported from Girl.glsl
// Copyright 2020 Inigo Quilez @iq - CC BY-NC-SA 3.0
// "Selfie Girl" - mathematical painting

#include "../common.hpp"

namespace sdf::animal {

namespace girl_detail {

inline float girl_smin(float a, float b, float k) {
    float h = glm::max(k - abs(a - b), 0.0f);
    return glm::min(a, b) - h * h * 0.25f / k;
}

inline float girl_smax(float a, float b, float k) {
    k *= 1.4f;
    float h = glm::max(k - abs(a - b), 0.0f);
    return glm::max(a, b) + h * h * h / (6.0f * k * k);
}

inline float smin3(float a, float b, float k) {
    k *= 1.4f;
    float h = glm::max(k - abs(a - b), 0.0f);
    return glm::min(a, b) - h * h * h / (6.0f * k * k);
}

inline float sclamp(float x, float a, float b) {
    float k = 0.1f;
    return girl_smax(girl_smin(x, b, k), a, k);
}

inline float opOnion(float sdf, float thickness) {
    return abs(sdf) - thickness;
}

inline float det(const vec2& a, const vec2& b) { return a.x * b.y - b.x * a.y; }
inline float ndot(const vec2& a, const vec2& b) { return a.x * b.x - a.y * b.y; }
inline float dot2(const vec2& v) { return dot(v, v); }
inline float dot2(const vec3& v) { return dot(v, v); }

inline float sdTorus(const vec3& p, float ra, float rb) {
    return length(vec2(length(vec2(p.x, p.z)) - ra, p.y)) - rb;
}

inline float sdCappedTorus(const vec3& p_in, const vec2& sc, float ra, float rb) {
    vec3 p = p_in;
    p.x = abs(p.x);
    float k = (sc.y * p.x > sc.x * p.z) ? dot(vec2(p.x, p.z), sc) : length(vec2(p.x, p.z));
    return sqrt(dot(p, p) + ra * ra - 2.0f * ra * k) - rb;
}

inline float sdSphere(const vec3& p, float r) {
    return length(p) - r;
}

inline float sdEllipsoid(const vec3& p, const vec3& r) {
    float k0 = length(p / r);
    float k1 = length(p / (r * r));
    return k0 * (k0 - 1.0f) / k1;
}

inline float sdBox(const vec3& p, const vec3& b) {
    vec3 d = abs(p) - b;
    return glm::min(glm::max(glm::max(d.x, d.y), d.z), 0.0f) + length(max(d, vec3(0.0f)));
}

inline float sdArc(const vec2& p_in, const vec2& scb, float ra) {
    vec2 p = vec2(abs(p_in.x), p_in.y);
    float k = (scb.y * p.x > scb.x * p.y) ? dot(p, scb) : length(p);
    return sqrt(dot(p, p) + ra * ra - 2.0f * ra * k);
}

inline vec4 sdBezier(const vec3& p, const vec3& va, const vec3& vb, const vec3& vc) {
    vec3 w = normalize(cross(vc - vb, va - vb));
    vec3 u = normalize(vc - vb);
    vec3 v = cross(w, u);
    
    vec2 m = vec2(dot(va - vb, u), dot(va - vb, v));
    vec2 n = vec2(dot(vc - vb, u), dot(vc - vb, v));
    vec3 q = vec3(dot(p - vb, u), dot(p - vb, v), dot(p - vb, w));
    
    float mn = det(m, n);
    float mq = det(m, vec2(q.x, q.y));
    float nq = det(n, vec2(q.x, q.y));
    
    vec2 g = (nq + mq + mn) * n + (nq + mq - mn) * m;
    float f = (nq - mq + mn) * (nq - mq + mn) + 4.0f * mq * nq;
    vec2 z = 0.5f * f * vec2(-g.y, g.x) / dot(g, g);
    float t = clamp(0.5f + 0.5f * (det(z - vec2(q.x, q.y), m + n)) / mn, 0.0f, 1.0f);
    vec2 cp = m * (1.0f - t) * (1.0f - t) + n * t * t - vec2(q.x, q.y);
    
    float d2 = dot(cp, cp);
    return vec4(sqrt(d2 + q.z * q.z), t, q.z, -sign(f) * sqrt(d2));
}

inline vec2 sdSegment(const vec3& p, const vec3& a, const vec3& b) {
    vec3 pa = p - a, ba = b - a;
    float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0f, 1.0f);
    return vec2(length(pa - ba * h), h);
}

inline float sdFakeRoundCone(const vec3& p_in, float b, float r1, float r2) {
    vec3 p = p_in;
    float h = clamp(p.y / b, 0.0f, 1.0f);
    p.y -= b * h;
    return length(p) - mix(r1, r2, h);
}

inline float sdCone(const vec3& p, const vec2& c) {
    vec2 q = vec2(length(vec2(p.x, p.z)), p.y);
    vec2 a = q - c * clamp((q.x * c.x + q.y * c.y) / dot(c, c), 0.0f, 1.0f);
    vec2 b = q - c * vec2(clamp(q.x / c.x, 0.0f, 1.0f), 1.0f);
    float s = -sign(c.y);
    vec2 d = min(vec2(dot(a, a), s * (q.x * c.y - q.y * c.x)),
                 vec2(dot(b, b), s * (q.y - c.y)));
    return -sqrt(d.x) * sign(d.y);
}

inline vec4 opElongate(const vec3& p, const vec3& h) {
    vec3 q = abs(p) - h;
    return vec4(max(q, vec3(0.0f)), glm::min(glm::max(q.x, glm::max(q.y, q.z)), 0.0f));
}

inline vec2 rot(const vec2& p, float an) {
    float cc = cos(an);
    float ss = sin(an);
    return mat2(cc, -ss, ss, cc) * p;
}

// Animation data (simplified - no animation for CPU version)
const vec3 animData = vec3(0.0f, 0.0f, 0.0f); // { blink, nose follow up, mouth }
const vec3 animHead = vec3(0.0f, 0.0f, 0.0f); // { head rotation angles }

inline vec4 sdHair(const vec3& p, const vec3& pa, const vec3& pb, const vec3& pc, float an, vec2& occ_id) {
    vec4 b = sdBezier(p, pa, pb, pc);
    vec2 q = rot(vec2(b.z, b.w), an);
    
    vec2 id2 = round(q / 0.1f);
    id2 = clamp(id2, vec2(0.0f), vec2(2.0f, 1.0f));
    q -= 0.1f * id2;
    
    float id = 11.0f * id2.x + id2.y * 13.0f;
    
    q += smoothstep(0.5f, 0.8f, b.y) * 0.02f * vec2(0.4f, 1.5f) * cos(23.0f * b.y + id * vec2(13.0f, 17.0f));
    
    occ_id.x = clamp(length(q) * 8.0f - 0.2f, 0.0f, 1.0f);
    vec4 res = vec4(99.0f, q.x, q.y, b.y);
    for (int i = 0; i < 3; i++) {
        vec2 tmp = q + 0.01f * cos(id + 180.0f * b.y + vec2(float(2 * i), float(6 - 2 * i)));
        float lt = length(tmp) - 0.02f;
        if (lt < res.x) {
            occ_id.y = id + float(i);
            res.x = lt;
            res.y = tmp.x;
            res.z = tmp.y;
        }
    }
    return res;
}

inline vec4 sdHoodie(const vec3& pos_in) {
    vec3 pos = pos_in;
    vec3 opos = pos;
    
    pos.x += 0.09f * sin(3.5f * pos.y - 0.5f) * sin(pos.z) + 0.015f;
    pos += 0.03f * sin(2.0f * pos.y) * sin(7.0f * vec3(pos.z, pos.x, pos.y));
    
    // hoodie
    vec3 hos = pos - vec3(0.0f, -0.33f, 0.15f);
    hos.x -= 0.031f * smoothstep(0.0f, 1.0f, opos.y + 0.33f);
    vec2 hosyz = rot(vec2(hos.y, hos.z), 0.9f);
    hos.y = hosyz.x; hos.z = hosyz.y;
    float d1 = sdEllipsoid(hos, vec3(0.96f - pos.y * 0.1f, 1.23f, 1.5f));
    float d2 = 0.95f * pos.z - 0.312f * pos.y - 0.9f;
    float d = glm::max(opOnion(d1, 0.01f), d2);
    
    // shoulders
    vec3 sos = vec3(abs(pos.x), pos.y, pos.z);
    vec2 se = sdSegment(sos, vec3(0.18f, -1.6f, -0.3f), vec3(1.1f, -1.9f, 0.0f));
    d = girl_smin(d, se.x - mix(0.25f, 0.43f, se.y), 0.4f);
    d = girl_smin(d, sdSphere(sos - vec3(0.3f, -2.2f, 0.4f), 0.5f), 0.2f);
    
    // neck
    opos.x -= 0.02f * sin(9.0f * opos.y);
    vec4 w = opElongate(opos - vec3(0.0f, -1.2f, 0.3f), vec3(0.0f, 0.3f, 0.0f));
    d = girl_smin(d, w.w + sdCappedTorus(vec3(w.x, w.y, -w.z), vec2(0.6f, -0.8f), 0.6f, 0.02f), 0.1f);
    
    // bumps
    d += 0.004f * sin(pos.x * 90.0f) * sin(pos.y * 90.0f) * sin(pos.z * 90.0f);
    d -= 0.002f * sin(pos.x * 300.0f);
    d -= 0.02f * (1.0f - smoothstep(0.0f, 0.04f, abs(opOnion(pos.x, 1.1f))));
    
    // border
    d = glm::min(d, length(vec2(d1, d2)) - 0.015f);
    
    return vec4(d, pos);
}

inline vec3 moveHead(const vec3& pos, const vec3& an, float amount) {
    vec3 p = pos;
    p.y -= -1.0f;
    vec2 pxz = rot(vec2(p.x, p.z), amount * an.x);
    p.x = pxz.x; p.z = pxz.y;
    vec2 pxy = rot(vec2(p.x, p.y), amount * an.y);
    p.x = pxy.x; p.y = pxy.y;
    vec2 pyz = rot(vec2(p.y, p.z), amount * an.z);
    p.y = pyz.x; p.z = pyz.y;
    p.y += -1.0f;
    return p;
}

inline vec4 mapGirl(const vec3& pos_in, float time, float& outMat, vec3& uvw) {
    outMat = 1.0f;
    
    vec3 pos = pos_in;
    pos.y /= 1.04f;
    vec3 opos = moveHead(pos, animHead, smoothstep(-1.2f, 0.2f, pos.y));
    pos = moveHead(pos, animHead, smoothstep(-1.4f, -1.0f, pos.y));
    pos.x *= 1.04f;
    pos.y /= 1.02f;
    uvw = pos;
    
    vec3 qos = vec3(abs(pos.x), pos.y, pos.z);
    vec3 sos = vec3(sqrt(qos.x * qos.x + 0.0005f), pos.y, pos.z);
    
    // head
    float d = sdEllipsoid(pos - vec3(0.0f, 0.05f, 0.07f), vec3(0.8f, 0.75f, 0.85f));
    
    // jaw
    vec3 mos = pos - vec3(0.0f, -0.38f, 0.35f);
    vec2 mosyz = rot(vec2(mos.y, mos.z), 0.4f);
    mos.y = mosyz.x; mos.z = mosyz.y;
    mosyz = rot(vec2(mos.y, mos.z), 0.1f * animData.z);
    mos.y = mosyz.x; mos.z = mosyz.y;
    float d2 = sdEllipsoid(mos - vec3(0.0f, -0.17f, 0.16f),
                 vec3(0.66f + sclamp(mos.y * 0.9f - 0.1f * mos.z, -0.3f, 0.4f),
                      0.43f + sclamp(mos.y * 0.5f, -0.5f, 0.2f),
                      0.50f + sclamp(mos.y * 0.3f, -0.45f, 0.5f)));
    
    // mouth hole
    d2 = girl_smax(d2, -sdEllipsoid(mos - vec3(0.0f, 0.06f, 0.6f + 0.05f * animData.z), 
                                    vec3(0.16f, 0.035f + 0.05f * animData.z, 0.1f)), 0.01f);
    
    // lower lip
    vec4 b = sdBezier(vec3(abs(mos.x), mos.y, mos.z),
                      vec3(0.0f, 0.01f, 0.61f),
                      vec3(0.094f + 0.01f * animData.z, 0.015f, 0.61f),
                      vec3(0.18f - 0.02f * animData.z, 0.06f + animData.z * 0.05f, 0.57f - 0.006f * animData.z));
    float isLip = smoothstep(0.045f, 0.04f, b.x + b.y * 0.03f);
    d2 = girl_smin(d2, b.x - 0.027f * (1.0f - b.y * b.y) * smoothstep(1.0f, 0.4f, b.y), 0.02f);
    d = girl_smin(d, d2, 0.19f);
    
    // chicks
    d = girl_smin(d, sdSphere(qos - vec3(0.2f, -0.33f, 0.62f), 0.28f), 0.04f);
    
    // eye sockets
    vec3 eos = sos - vec3(0.3f, -0.04f, 0.7f);
    vec2 eosxz = rot(vec2(eos.x, eos.z), -0.2f);
    eos.x = eosxz.x; eos.z = eosxz.y;
    vec2 eosxy = rot(vec2(eos.x, eos.y), 0.3f);
    eos.x = eosxy.x; eos.y = eosxy.y;
    vec2 eosyz = rot(vec2(eos.y, eos.z), -0.2f);
    eos.y = eosyz.x; eos.z = eosyz.y;
    d2 = sdEllipsoid(eos - vec3(-0.05f, -0.05f, 0.2f), vec3(0.20f, 0.14f - 0.06f * animData.x, 0.1f));
    d = girl_smax(d, -d2, 0.15f);
    
    eos = sos - vec3(0.32f, -0.08f, 0.8f);
    eosxz = rot(vec2(eos.x, eos.z), -0.4f);
    eos.x = eosxz.x; eos.z = eosxz.y;
    d2 = sdEllipsoid(eos, vec3(0.154f, 0.11f, 0.1f));
    d = girl_smax(d, -d2, 0.05f);
    
    // nose
    eos = pos - vec3(0.0f, -0.079f + animData.y * 0.005f, 0.86f);
    eosyz = rot(vec2(eos.y, eos.z), -0.23f);
    eos.y = eosyz.x; eos.z = eosyz.y;
    float h = smoothstep(0.0f, 0.26f, -eos.y);
    d2 = sdCone(eos - vec3(0.0f, -0.02f, 0.0f), vec2(0.03f, -0.25f)) - 0.04f * h - 0.01f;
    eos.x = sqrt(eos.x * eos.x + 0.001f);
    d2 = girl_smin(d2, sdSphere(eos - vec3(0.0f, -0.25f, 0.037f), 0.06f), 0.07f);
    d2 = girl_smin(d2, sdSphere(eos - vec3(0.1f, -0.27f, 0.03f), 0.04f), 0.07f);
    d2 = girl_smin(d2, sdSphere(eos - vec3(0.0f, -0.32f, 0.05f), 0.025f), 0.04f);
    d2 = girl_smax(d2, -sdSphere(eos - vec3(0.07f, -0.31f, 0.038f), 0.02f), 0.035f);
    d = girl_smin(d, d2, 0.05f - 0.03f * h);
    
    // neck
    vec2 se = sdSegment(pos, vec3(0.0f, -0.65f, 0.0f), vec3(0.0f, -1.7f, -0.1f));
    d2 = se.x - 0.38f;
    
    // shoulders
    se = sdSegment(sos, vec3(0.0f, -1.55f, 0.0f), vec3(0.6f, -1.65f, 0.0f));
    d2 = girl_smin(d2, se.x - 0.21f, 0.1f);
    d = girl_smin(d, d2, 0.4f);
    
    vec4 res = vec4(d, isLip, 0.0f, 0.0f);
    
    // eyes
    pos.x /= 1.05f;
    eos = qos - vec3(0.25f, -0.06f, 0.42f);
    d2 = sdSphere(eos, 0.4f);
    if (d2 < res.x) {
        res.x = d2;
        outMat = 2.0f;
        uvw = pos;
    }
    
    // hair
    {
        vec2 occ_id, tmp;
        qos = pos;
        qos.x = abs(pos.x);
        
        vec4 pres = sdHair(pos, vec3(-0.3f, 0.55f, 0.8f),
                           vec3(0.95f, 0.7f, 0.85f),
                           vec3(0.4f, -1.45f, 0.95f),
                           -0.9f, occ_id);
        
        vec4 pres2 = sdHair(pos, vec3(-0.4f, 0.6f, 0.55f),
                            vec3(-1.0f, 0.4f, 0.2f),
                            vec3(-0.6f, -1.4f, 0.7f),
                            0.6f, tmp);
        if (pres2.x < pres.x) { pres = pres2; occ_id = tmp; occ_id.y += 40.0f; }
        
        pres2 = sdHair(qos, vec3(0.4f, 0.7f, 0.4f),
                       vec3(1.0f, 0.5f, 0.45f),
                       vec3(0.4f, -1.45f, 0.55f),
                       -0.2f, tmp);
        if (pres2.x < pres.x) { pres = pres2; occ_id = tmp; occ_id.y += 80.0f; }
        
        pres.x *= 0.8f;
        if (pres.x < res.x) {
            res = vec4(pres.x, occ_id.y, 0.0f, occ_id.x);
            uvw = vec3(pres.y, pres.z, pres.w);
            outMat = 4.0f;
        }
    }
    
    // hoodie
    vec4 hoodie = sdHoodie(opos);
    if (hoodie.x < res.x) {
        res.x = hoodie.x;
        outMat = 3.0f;
        uvw = vec3(hoodie.y, hoodie.z, hoodie.w);
    }
    
    return res;
}

} // namespace girl_detail

inline float Girl(const vec3& p_in, float time, uint32_t /*seed*/) {
    vec3 p = p_in + vec3(0.0f, -0.2f, 0.0f);
    float boxD = girl_detail::sdBox(p, vec3(1.0f, 1.0f, 1.0f));
    const float scale = 0.6f;
    p *= 1.0f / scale;
    float matID;
    vec3 uvw;
    return glm::max(boxD, girl_detail::mapGirl(p, time, matID, uvw).x) * 0.5f;
}

} // namespace sdf::animal

