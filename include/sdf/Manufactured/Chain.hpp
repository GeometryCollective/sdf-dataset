#pragma once
// Ported from Chain.glsl
// Copyright 2019 @eiffie - MIT License

#include "../common.hpp"

namespace sdf {
namespace manufactured {

namespace chain_detail {
    constexpr float TWISTS = 4.5f;
    constexpr float TAO = 6.2831853f;
    constexpr float pdt = 10.0f / TAO;
    constexpr float tdp = TAO / 10.0f;
    
    inline vec2 chain_Rot2D(const vec2& v, float angle) {
        return std::cos(angle) * v + std::sin(angle) * vec2(v.y, -v.x);
    }
    
    inline float Link(vec3 p, float a) {
        vec2 pxy = chain_Rot2D(vec2(p.x, p.y), a);
        p.x = pxy.x;
        p.y = pxy.y;
        p.y += 1.0f + std::sin(a + 60.0f) * 0.2f;
        vec2 pyz = chain_Rot2D(vec2(p.y, p.z), a * TWISTS + 60.0f);
        p.y = pyz.x;
        p.z = pyz.y;
        return length(vec2(length(max(abs(vec2(p.x, p.y)) - vec2(0.125f, 0.025f), vec2(0.0f))) - 0.1f, p.z)) - 0.02f;
    }
    
    inline float DE(const vec3& p) {
        float a = std::atan2(p.x, -p.y) * pdt;
        return std::min(Link(p, std::floor(0.5f + a) * tdp), Link(p, (std::floor(a) + 0.5f) * tdp));
    }
}

inline float Chain(const vec3& p, float /*time*/, uint32_t /*seed*/) {
    vec3 pTrans = p + vec3(-0.11f, 0.0f, 0.0f);
    const float scale = 0.7f;
    return chain_detail::DE(pTrans * (1.0f / scale)) * scale * 0.6f;
}

} // namespace manufactured
} // namespace sdf

