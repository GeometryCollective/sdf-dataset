#pragma once
// Ported from Fish.glsl
// Copyright 2016 Martijn Steinrucken @BigWings - CC BY-NC-SA 3.0

#include "../common.hpp"

namespace sdf::animal {

namespace detail {
    inline float B(float x, float y, float z, float w) {
        return smoothstep(x - z, x + z, w) * smoothstep(y + z, y - z, w);
    }
    
    inline float SIN(float x) { return sin(x) * 0.5f + 0.5f; }
    
    inline float L2(const vec3& p) { return dot(p, p); }
    inline float L2(const vec2& p) { return dot(p, p); }
    
    inline float N1(float x) { return fract(sin(x) * 5346.1764f); }
    inline float N2(float x, float y) { return N1(x + y * 134.324f); }
    
    inline float remap01(float a, float b, float t) { return (t - a) / (b - a); }
    
    inline float fmin(float a, float b, float k, float f, float amp) {
        float h = clamp(0.5f + 0.5f * (b - a) / k, 0.0f, 1.0f);
        float scale = h * (1.0f - h);
        return mix(b, a, h) - (k + std::cos(h * pi * f) * amp * k) * scale;
    }
    
    inline float scaleSphere(const vec3& p, const vec3& scale, float s) {
        return (length(p / scale) - s) * std::min(scale.x, std::min(scale.y, scale.z));
    }
    
    struct de {
        float d, b, m, a, a2;
        float d1, d2, d3, d4, d5;
        vec3 p;
        vec3 s1;
    };
    
    inline vec3 Scales(const vec2& uv, float seed) {
        vec2 uv2 = fract(uv);
        vec2 uv3 = floor(uv);
        
        float rDist = length(uv2 - vec2(1.0f, 0.5f));
        float rMask = smoothstep(0.5f, 0.45f, rDist);
        float rN = N2(uv3.x, uv3.y + seed);
        vec3 rCol = vec3(uv2.x - 0.5f, rN, rDist);
        
        float tDist = length(uv2 - vec2(0.5f, 1.0f));
        float tMask = smoothstep(0.5f, 0.45f, tDist);
        float tN = N2(uv3.x, uv3.y + seed);
        vec3 tCol = vec3(1.0f * uv2.x, tN, tDist);
        
        float bDist = length(uv2 - vec2(0.5f, 0.0f));
        float bMask = smoothstep(0.5f, 0.45f, bDist);
        float bN = N2(uv3.x, uv3.y - 1.0f + seed);
        vec3 bCol = vec3(uv2.x, bN, bDist);
        
        float lDist = length(uv2 - vec2(0.0f, 0.5f));
        float lMask = smoothstep(0.5f, 0.45f, lDist);
        float lN = N2(uv3.x - 1.0f, uv3.y + seed);
        vec3 lCol = vec3(uv2.x + 0.5f, lN, lDist);
        
        vec3 col = rMask * rCol;
        col = mix(col, tCol, tMask);
        col = mix(col, bCol, bMask);
        col = mix(col, lCol, lMask);
        
        return col;
    }
    
    inline de Fish(vec3 p, const vec3& n, float camDist, float time) {
        const vec3 lf = vec3(1.0f, 0.0f, 0.0f);
        const vec3 up = vec3(0.0f, 1.0f, 0.0f);
        const vec3 fw = vec3(0.0f, 0.0f, 1.0f);
        
        p.x += 1.5f;
        p.z += sin(p.x - time * 2.0f + n.x * 100.0f) * mix(0.15f, 0.25f, n.y);
        p.z = std::abs(p.z);
        
        float fadeDetail = smoothstep(25.0f, 5.0f, camDist);
        
        vec3 P;
        float dist, mask, r;
        vec2 dR;
        float bump = 0.0f;
        
        float lobe = scaleSphere(p - vec3(-1.0f, 0.0f, 0.25f), vec3(1.0f, 1.0f, 0.5f), 0.4f);
        float lobe2 = scaleSphere(p - vec3(-1.0f, 0.0f, -0.25f), vec3(1.0f, 1.0f, 0.5f), 0.4f);
        
        vec3 eyePos = p - vec3(-1.0f, 0.0f, 0.4f);
        float eye = scaleSphere(eyePos, vec3(1.0f, 1.0f, 0.35f), 0.25f);
        float eyeAngle = std::atan2(eyePos.x, eyePos.y);
        
        float snout = scaleSphere(p - vec3(-1.2f, -0.2f, 0.0f), vec3(1.5f, 1.0f, 0.5f), 0.4f);
        P = p - vec3(-1.2f, -0.6f, 0.0f);
        P = rotationMatrix(vec3(0.0f, 0.0f, 1.0f), 0.35f) * P;
        float jawDn = scaleSphere(P, vec3(1.0f, 0.2f, 0.4f), 0.6f);
        float jawUp = scaleSphere(P - vec3(-0.3f, 0.15f, 0.0f), vec3(0.6f, 0.2f, 0.3f), 0.6f);
        float mouth = fmin(jawUp, jawDn, 0.03f, 5.0f, 0.1f);
        snout = smin(snout, mouth, 0.1f);
        
        float body1 = scaleSphere(p - vec3(0.6f, 0.0f, 0.0f), vec3(2.0f, 1.0f, 0.5f), 1.0f);
        float body2 = scaleSphere(p - vec3(2.4f, 0.1f, 0.0f), vec3(3.0f, 1.0f, 0.4f), 0.6f);
        
        P = p - vec3(-1.0f, 0.0f, 0.0f);
        float angle = std::atan2(P.y, P.z);
        vec2 uv = vec2(remap01(-2.0f, 3.0f, p.x), (angle / pi) + 0.5f);
        vec2 uv2 = uv * vec2(2.0f, 1.0f) * 20.0f;
        
        vec3 sInfo = Scales(uv2, n.z);
        float scales = -(sInfo.x - sInfo.z * 2.0f) * 0.01f;
        scales *= smoothstep(0.33f, 0.45f, eye) * smoothstep(1.8f, 1.2f, eye) * smoothstep(-0.3f, 0.0f, p.x);
        
        // Gill plates
        P = p - vec3(-0.7f, -0.25f, 0.2f);
        P = rotationMatrix(vec3(0.0f, 1.0f, 0.0f), 0.4f) * P;
        float gill = scaleSphere(P, vec3(1.0f, 0.9f, 0.15f), 0.8f);
        
        // Fins
        float tail = scaleSphere(p - vec3(4.5f, 0.1f, 0.0f), vec3(1.0f, 2.0f, 0.2f), 0.5f);
        dR = vec2(p.x, p.y) - vec2(3.8f, 0.1f);
        r = std::atan2(dR.x, dR.y);
        
        mask = B(0.45f, 2.9f, 0.2f, r) * smoothstep(0.2f * 0.2f, 1.0f, L2(dR));
        
        bump += std::sin(r * 70.0f) * 0.005f * mask;
        tail += (std::sin(r * 5.0f) * 0.03f + bump) * mask;
        tail += std::sin(r * 280.0f) * 0.001f * mask * fadeDetail;
        
        float dorsal1 = scaleSphere(p - vec3(1.5f, 1.0f, 0.0f), vec3(3.0f, 1.0f, 0.2f), 0.5f);
        float dorsal2 = scaleSphere(p - vec3(0.5f, 1.5f, 0.0f), vec3(1.0f, 1.0f, 0.1f), 0.5f);
        dR = vec2(p.x, p.y);
        r = std::atan2(dR.x, dR.y);
        dorsal1 = smin(dorsal1, dorsal2, 0.1f);
        
        mask = B(-0.2f, 3.0f, 0.2f, p.x);
        bump += std::sin(r * 100.0f) * 0.003f * mask;
        bump += (1.0f - std::pow(std::sin(r * 50.0f) * 0.5f + 0.5f, 15.0f)) * 0.015f * mask;
        bump += std::sin(r * 400.0f) * 0.001f * mask * fadeDetail;
        dorsal1 += bump;
        
        float anal = scaleSphere(p - vec3(2.6f, -0.7f, 0.0f), vec3(2.0f, 0.7f, 0.1f), 0.5f);
        anal += std::sin(r * 300.0f) * 0.001f;
        anal += std::sin(r * 40.0f) * 0.01f;
        
        // Arm fins
        P = p - vec3(0.7f, -0.6f, 0.55f);
        dR = vec2(p.x, p.y) - vec2(0.3f, -0.4f);
        r = std::atan2(dR.x, dR.y);
        P = rotationMatrix(lf, 0.2f) * P;
        P = rotationMatrix(up, 0.2f) * P;
        mask = B(1.5f, 2.9f, 0.1f, r);
        mask *= smoothstep(0.1f * 0.1f, 0.6f * 0.6f, L2(dR));
        float arm = scaleSphere(P, vec3(2.0f, 1.0f, 0.2f), 0.2f);
        arm += (std::sin(r * 10.0f) * 0.01f + std::sin(r * 100.0f) * 0.002f) * mask;
        
        // Breast fins
        P = p - vec3(0.9f, -1.1f, 0.2f);
        P = rotationMatrix(fw, 0.4f) * P;
        P = rotationMatrix(lf, 0.4f) * P;
        dR = vec2(p.x, p.y) - vec2(0.5f, -0.9f);
        r = std::atan2(dR.x, dR.y);
        mask = B(1.5f, 2.9f, 0.1f, r);
        mask *= smoothstep(0.1f * 0.1f, 0.4f * 0.4f, L2(dR));
        float breast = scaleSphere(P, vec3(2.0f, 1.0f, 0.2f), 0.2f);
        breast += (std::sin(r * 10.0f) * 0.01f + std::sin(r * 60.0f) * 0.002f) * mask;
        
        de f;
        f.p = p;
        f.a = angle;
        f.a2 = eyeAngle;
        f.d4 = length(eyePos);
        f.m = 1.0f;
        
        f.d1 = smin(lobe, lobe2, 0.2f);
        f.d1 = smin(f.d1, snout, 0.3f);
        f.d1 += 0.005f * (std::sin(f.a2 * 20.0f + f.d4) * std::sin(f.a2 * 3.0f + f.d4 * -4.0f) * SIN(f.d4 * 10.0f));
        f.d1 = smin(f.d1, body1, 0.15f);
        f.d1 = smin(f.d1, body2, 0.3f);
        f.d1 += scales * fadeDetail;
        f.d1 = fmin(f.d1, gill, 0.1f, 5.0f, 0.1f);
        
        float fins = std::min(arm, breast);
        fins = std::min(fins, tail);
        fins = std::min(fins, dorsal1);
        fins = std::min(fins, anal);
        
        f.d = smin(f.d1, fins, 0.05f);
        f.d = fmin(f.d, eye, 0.01f, 2.0f, 1.0f);
        f.d *= 0.8f;
        
        f.d2 = dorsal1;
        f.d3 = tail;
        f.d5 = mouth;
        f.b = bump;
        f.s1 = sInfo;
        
        return f;
    }
}

inline float Fish(const vec3& p_in, float time, uint32_t /*seed*/) {
    vec3 p = p_in + vec3(0.0f, 0.1f, 0.0f);
    p = rotationMatrix(vec3(0.0f, 1.0f, 0.0f), -pi / 2.0f) * p;
    const float scale = 0.22f;
    p *= (1.0f / scale);
    return detail::Fish(p, vec3(0.0f), 0.0f, time).d * scale;
}

} // namespace sdf::animal


