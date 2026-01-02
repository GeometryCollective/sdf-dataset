#pragma once
// Ported from Cheese.glsl
// Copyright 2018 Felipe Alfonso @bitnenfer - CC BY-NC-SA 3.0

#include "../common.hpp"

namespace sdf {
namespace misc {

namespace cheese_detail {
    inline void opRotate(vec2& v, float r) {
        float c = std::cos(r);
        float s = std::sin(r);
        float vx = v.x * c - v.y * s;
        float vy = v.x * s + v.y * c;
        v.x = vx;
        v.y = vy;
    }
    
    inline vec3 opRepeate(const vec3& p, const vec3& c) { return mod(p, c) - 0.5f * c; }
    
    inline float opDisp(const vec3& p) {
        return std::sin(20.0f * p.x) * std::sin(20.0f * p.y) * std::sin(20.0f * p.z);
    }
    
    inline float sdBox(const vec3& p, const vec3& b) {
        vec3 d = abs(p) - b;
        return length(max(d, vec3(0.0f))) + std::min(std::max(d.x, std::max(d.y, d.z)), 0.0f);
    }
    
    inline float sdSphere(const vec3& p, float r) { return length(p) - r; }
    
    inline float sdRoundedCylinder(const vec3& p, float ra, float rb, float h) {
        vec2 d = vec2(length(vec2(p.x, p.z)) - 2.0f * ra + rb, std::abs(p.y) - h);
        return std::min(std::max(d.x, d.y), 0.0f) + length(max(d, vec2(0.0f))) - rb;
    }
    
    inline float sdTriPrism(const vec3& p, const vec2& h) {
        vec3 q = abs(p);
        return std::max(q.z - h.y, std::max(q.x * 0.866025f + p.y * 0.5f, -p.y) - h.x * 0.5f);
    }
    
    inline float sdCappedCylinder(const vec3& p, const vec2& h) {
        vec2 d = abs(vec2(length(vec2(p.x, p.z)), p.y)) - h;
        return std::min(std::max(d.x, d.y), 0.0f) + length(max(d, vec2(0.0f)));
    }
    
    inline float mapCheese(vec3 p) {
        float value = 0.0f;
        vec2 pxz = vec2(p.x, p.z);
        opRotate(pxz, 0.5f);
        p.x = pxz.x; p.z = pxz.y;
        
        // base shape
        float cyl0 = sdRoundedCylinder(p, 0.30f, 0.04f, 0.3f);
        vec3 triP0 = p + vec3(0.0f, 0.0f, -0.2f);
        vec2 trizy = vec2(triP0.z, triP0.y);
        opRotate(trizy, -pi / 2.0f);
        triP0.z = trizy.x; triP0.y = trizy.y;
        float tri0 = sdTriPrism(triP0, vec2(0.8f, 0.4f)) - 0.001f;
        value = std::max(tri0, cyl0);
        
        // holes
        vec3 holesP = p + vec3(-0.1119641f, -0.6300012f, 0.1603024f);
        holesP = opRepeate(holesP, vec3(0.45f));
        float sp = sdSphere(holesP, 0.12f);
        sp += opDisp(p * 0.3f) * 0.15f;
        value = std::max(-sp, value);
        
        // cut
        vec3 cutP = p + vec3(0.0f, 0.0f, 0.62f);
        vec2 cutxz = vec2(cutP.x, cutP.z);
        opRotate(cutxz, -0.4f);
        cutP.x = cutxz.x; cutP.z = cutxz.y;
        vec2 cutyz = vec2(cutP.y, cutP.z);
        opRotate(cutyz, -0.2f);
        cutP.y = cutyz.x; cutP.z = cutyz.y;
        cutP = opRepeate(cutP, vec3(0.09f, 0.001f, 0.0f));
        float ct = sdSphere(cutP, 0.07f);
        
        value = std::max(-ct, value);
        
        return value;
    }
    
    inline float mapPlate(const vec3& p) {
        float base = sdBox(p, vec3(0.8f, -0.001f, 0.8f)) - 0.1f;
        float handle = sdBox(p + vec3(-1.0f, 0.0f, 0.0f), vec3(0.5f, 0.0f, 0.04f * std::sin(-0.1f + -p.x * 2.0f))) - 0.1f;
        return std::min(base, handle);
    }
    
    inline float mapKnife(vec3 p) {
        p *= 0.6f;
        vec3 handleP = p;
        vec2 hxy = vec2(handleP.x, handleP.y);
        opRotate(hxy, -std::sin(handleP.y) * 0.5f);
        handleP.x = hxy.x; handleP.y = hxy.y;
        opRotate(hxy, 0.14f);
        handleP.x = hxy.x; handleP.y = hxy.y;
        float handle = sdCappedCylinder(handleP, vec2(0.0f, 0.24f)) - std::abs(0.05f * std::cos(0.3f + p.y * -4.0f));
        float blade = sdBox(p + vec3(0.05f, -0.6f, 0.0f), vec3(0.1f * std::sqrt(std::abs(std::cos(p.y * 1.57f))), 0.4f, 0.002f));
        return std::min(handle, blade);
    }
    
    inline float mapScene(vec3 p) {
        p = p + vec3(0.2f, 0.0f, 0.0f);
        vec3 cheeseP = p + vec3(0.3f, 0.0f, -0.1f);
        vec3 plateP = p + vec3(0.0f, 0.456f, 0.0f);
        vec3 knifeP = p + vec3(-0.7742586f, 0.2902968f, -0.11406922f);
        
        vec2 kyz = vec2(knifeP.y, knifeP.z);
        opRotate(kyz, pi / 2.0f + 0.015f);
        knifeP.y = kyz.x; knifeP.z = kyz.y;
        vec2 kxy = vec2(knifeP.x, knifeP.y);
        opRotate(kxy, -pi / 4.0f);
        knifeP.x = kxy.x; knifeP.y = kxy.y;
        vec2 kxz = vec2(knifeP.x, knifeP.z);
        opRotate(kxz, -0.28f);
        knifeP.x = kxz.x; knifeP.z = kxz.y;
        
        float cheese = mapCheese(cheeseP);
        float plate = mapPlate(plateP);
        float knife = mapKnife(knifeP);
        
        return std::min(knife, std::min(cheese, plate));
    }
}

inline float Cheese(const vec3& p, float /*time*/, uint32_t /*seed*/) {
    const float scale = 0.5f;
    vec3 pRot = rotationMatrix(vec3(0.0f, 1.0f, 0.0f), pi) * (p * (1.0f / scale));
    return cheese_detail::mapScene(pRot) * scale;
}

} // namespace misc
} // namespace sdf

