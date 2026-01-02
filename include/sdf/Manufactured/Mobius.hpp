#pragma once
// Ported from Mobius.glsl
// Copyright 2015 Gary Warne @Shane - CC BY-NC-SA 3.0

#include "../common.hpp"

namespace sdf {
namespace manufactured {

namespace mobius_detail {
    inline mat2 r2(float th) {
        vec2 a = sin(vec2(1.5707963f, 0.0f) + vec2(th, th));
        return mat2(a.x, a.y, -a.y, a.x);
    }
    
    inline float Mobius(vec3 q) {
        const float toroidRadius = 1.25f;
        const float polRot = 4.0f / 4.0f;
        const float ringNum = 32.0f;
        
        vec3 p = q;
        
        float a = std::atan2(p.z, p.x);
        
        float ia = std::floor(ringNum * a / 6.2831853f);
        ia = (ia + 0.5f) / ringNum * 6.2831853f;
        
        vec2 pxz = r2(a) * vec2(p.x, p.z);
        p.x = pxz.x; p.z = pxz.y;
        p.x -= toroidRadius;
        vec2 pxy = r2(a * polRot) * vec2(p.x, p.y);
        p.x = pxy.x; p.y = pxy.y;
        
        p = abs(abs(p) - vec3(0.25f));
        
        float rail = std::max(std::max(p.x, p.y) - 0.07f, 
            (std::max(p.y - p.x, p.y + p.x) * 0.7071f - 0.075f));
        
        p = q;
        pxz = r2(ia) * vec2(p.x, p.z);
        p.x = pxz.x; p.z = pxz.y;
        p.x -= toroidRadius;
        pxy = r2(a * polRot) * vec2(p.x, p.y);
        p.x = pxy.x; p.y = pxy.y;
        
        p = abs(p);
        float ring = std::max(p.x, p.y);
        ring = std::max(std::max(ring - 0.275f, p.z - 0.03f), -(ring - 0.2f));
        
        return smin(ring, rail, 0.07f);
    }
}

inline float Mobius(const vec3& p, float /*time*/, uint32_t /*seed*/) {
    const float scale = 0.5f;
    return mobius_detail::Mobius(p * (1.0f / scale)) * scale;
}

} // namespace manufactured
} // namespace sdf

