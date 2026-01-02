#pragma once
// Ported from Cybertruck.glsl
// Copyright 2019 Martijn Steinrucken @BigWings - CC BY-NC-SA 3.0

#include "../common.hpp"

namespace sdf {
namespace vehicle {

namespace cybertruck_detail {
    inline vec2 pModPolar(vec2& p, float repetitions, float fix) {
        float angle = 6.2832f / repetitions;
        float a = std::atan2(p.y, p.x) + angle / 2.0f;
        float r = length(p);
        float c = std::floor(a / angle);
        a = mod(a, angle) - (angle / 2.0f) * fix;
        p = vec2(std::cos(a), std::sin(a)) * r;
        return p;
    }
    
    inline float sabs(float x, float k) {
        float a = (0.5f / k) * x * x + k * 0.5f;
        float b = std::abs(x);
        return b < k ? a : b;
    }
    
    inline float sdCylinder(const vec3& p, const vec3& a, const vec3& b, float r) {
        vec3 ab = b - a;
        vec3 ap = p - a;
        float t = dot(ab, ap) / dot(ab, ab);
        vec3 c = a + t * ab;
        float x = length(p - c) - r;
        float y = (std::abs(t - 0.5f) - 0.5f) * length(ab);
        float e = length(max(vec2(x, y), vec2(0.0f)));
        float i = std::min(std::max(x, y), 0.0f);
        return e + i;
    }
    
    inline float sdBox(const vec3& p, const vec3& s) {
        vec3 q = abs(p) - s;
        return length(max(q, vec3(0.0f))) + std::min(std::max(q.x, std::max(q.y, q.z)), 0.0f);
    }
    
    inline float map01(float a, float b, float t) {
        return clamp((t - a) / (b - a), 0.0f, 1.0f);
    }
    
    inline float S(float a, float b, float t) {
        return smoothstep(a, b, t);
    }
    
    inline mat2 Rot(float a) {
        float s = std::sin(a);
        float c = std::cos(a);
        return mat2(c, -s, s, c);
    }
    
    inline float sdCar(vec3 p) {
        p.x = sabs(p.x, 0.5f);
        vec2 P = vec2(p.y, p.z);
        
        float d;
        float frontGlass = dot(P, vec2(0.9493f, 0.3142f)) - 1.506f;
        d = frontGlass;
        
        float topGlass = dot(P, vec2(0.9938f, -0.1110f)) - 1.407f;
        d = std::max(d, topGlass);
        float back = dot(P, vec2(0.9887f, -0.16f)) - 1.424f;
        d = std::max(d, back);
        
        float side1 = dot(p, vec3(0.9854f, -0.1696f, -0.0137f)) - 0.580f;
        d = std::max(d, side1);
        
        float side2 = dot(p, vec3(0.9661f, 0.2583f, 0.0037f)) - 0.986f;
        d = smin(d, side2, -0.005f);
        d = std::max(d, dot(P, vec2(-0.1578f, -0.9875f)) - 2.056f);
        d = std::max(d, dot(p, vec3(0.0952f, -0.1171f, 0.9885f)) - 2.154f);
        d = std::max(d, dot(p, vec3(0.5019f, -0.1436f, 0.8529f)) - 2.051f);
        d = std::max(d, dot(P, vec2(-0.9999f, -0.0118f)) + 0.2643f);
        d = std::max(d, dot(p, vec3(0.0839f, -0.4614f, 0.8832f)) - 1.770f);
        d = std::max(d, dot(p, vec3(0.0247f, -0.9653f, 0.2599f)) - 0.196f);
        d = std::max(d, dot(P, vec2(-0.9486f, -0.3163f)) - 0.295f);
        
        float car = d;
        
        // fenders
        d = dot(p, vec3(0.4614f, 0.3362f, 0.8210f)) - 2.2130f;
        d = std::max(d, dot(p, vec3(0.4561f, 0.8893f, 0.0347f)) - 1.1552f);
        d = std::max(d, dot(p, vec3(0.4792f, 0.3783f, -0.7920f)) + 0.403f);
        d = std::max(d, dot(p, vec3(0.4857f, -0.0609f, -0.8720f)) + 0.6963f);
        d = std::max(d, dot(p, vec3(0.4681f, -0.4987f, 0.7295f)) - 1.545f);
        d = std::max(d, 0.3f - p.y);
        d = std::max(d, std::abs(p.x - 0.62f - p.y * 0.15f) - 0.07f);
        car = std::min(car, d);
        
        // back fender
        d = dot(p, vec3(0.4943f, -0.0461f, 0.8681f)) + 0.4202f;
        d = std::max(d, dot(p, vec3(0.4847f, 0.4632f, 0.7420f)) + 0.0603f);
        d = std::max(d, dot(p, vec3(0.4491f, 0.8935f, 0.0080f)) - 1.081f);
        d = std::max(d, dot(p, vec3(0.3819f, 0.4822f, -0.7885f)) - 1.973f);
        d = std::max(d, std::min(0.58f - p.y, -1.5f - p.z));
        d = std::max(d, 0.3f - p.y);
        d = std::max(d, std::abs(side1 + 0.01f) - 0.08f);
        car = std::min(car, d);
        
        // wheel wells
        d = p.z - 2.0635f;
        d = std::max(d, dot(vec2(p.y, p.z), vec2(0.5285f, 0.8489f)) - 2.0260f);
        d = std::max(d, dot(vec2(p.y, p.z), vec2(0.9991f, 0.0432f)) - 0.8713f);
        d = std::max(d, dot(vec2(p.y, p.z), vec2(0.5258f, -0.8506f)) + 0.771f);
        d = std::max(d, 1.194f - p.z);
        d = std::max(d, 0.5f - p.x);
        car = std::max(car, -d);
        
        d = p.z + 0.908f;
        d = std::max(d, dot(vec2(p.y, p.z), vec2(0.5906f, 0.8070f)) + 0.434f);
        d = std::max(d, dot(vec2(p.y, p.z), vec2(0.9998f, 0.0176f)) - 0.7843f);
        d = std::max(d, dot(p, vec3(-0.0057f, 0.5673f, -0.8235f)) - 1.7892f);
        d = std::max(d, -p.z - 1.7795f);
        d = std::max(d, 0.5f - p.x);
        car = std::max(car, -d);
        
        return car;
    }
    
    inline float sdWheel(vec3 p) {
        vec3 wp = p;
        float w = sdCylinder(wp, vec3(-0.1f, 0.0f, 0.0f), vec3(0.1f, 0.0f, 0.0f), 0.32f) - 0.03f;
        
        wp *= 0.95f;
        vec2 pyz = vec2(wp.y, wp.z);
        pModPolar(pyz, 7.0f, 1.0f);
        wp.y = pyz.x; wp.z = pyz.y;
        
        float cap = std::max(p.x - 0.18f, wp.y - 0.3f);
        wp.z = std::abs(wp.z);
        
        float d = map01(0.3f, 0.23f, wp.y);
        d *= map01(0.04f, 0.03f, wp.z);
        d *= map01(-0.23f, 0.23f, wp.y) * 0.7f;
        d = std::max(d, map01(0.13f, 0.0f, wp.y) * 1.5f);
        d = std::min(d, map01(0.0f, 0.07f, wp.y));
        d = std::max(d, 0.8f * step(wp.y, 0.05f));
        d = std::max(d, 0.4f * map01(0.23f, 0.22f, dot(vec2(wp.z, wp.y), normalize(vec2(1.0f, 2.0f)))));
        
        cap += (1.0f - d) * 0.07f;
        cap = std::max(cap, 0.05f - p.x);
        cap *= 0.8f;
        
        w = std::min(w, cap);
        float dist = length(vec2(wp.z, wp.y));
        w += S(0.3f, 0.0f, dist) * 0.025f;
        
        return w;
    }
}

inline float Cybertruck(const vec3& p, float /*time*/, uint32_t /*seed*/) {
    vec3 pTrans = p;
    pTrans.y += 0.2f;
    const float scale = 0.35f;
    pTrans *= (1.0f / scale);
    
    float car = cybertruck_detail::sdCar(pTrans) * 0.8f * scale;
    vec3 wp = pTrans - vec3(0.0f, 0.0f, 0.14f);
    wp.x = std::abs(wp.x);
    wp.z = std::abs(wp.z);
    wp -= vec3(0.7383f, 0.365f, 1.5f);
    
    if (pTrans.z > 0.0f) {
        mat2 rot = cybertruck_detail::Rot(0.3f * sign(pTrans.x));
        vec2 wxz = rot * vec2(wp.x, wp.z);
        wp.x = wxz.x; wp.z = wxz.y;
    }
    float wheel = cybertruck_detail::sdWheel(wp) * scale;
    
    return std::min(car, wheel);
}

} // namespace vehicle
} // namespace sdf

