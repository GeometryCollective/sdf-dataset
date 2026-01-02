#pragma once
// Ported from Trefoil.glsl
// Copyright 2018 @dr2 - CC BY-NC-SA 3.0

#include "../common.hpp"

namespace sdf::geometry {

namespace detail {
    inline float PrBox2Df(const vec2& p, const vec2& b) {
        vec2 d = abs(p) - b;
        return std::min(std::max(d.x, d.y), 0.0f) + length(max(d, vec2(0.0f)));
    }
    
    inline vec2 Rot2D(const vec2& q, float a) {
        return q * std::cos(a) + vec2(q.y, q.x) * std::sin(a) * vec2(-1.0f, 1.0f);
    }
    
    inline float ObjDf(const vec3& p, float r) {
        const float dstFar = 100.0f;
        float dMin = dstFar;
        
        vec3 q = p;
        float a = std::atan2(q.z, q.x);
        vec2 qxz = vec2(length(vec2(q.x, q.z)) - r, q.y);
        qxz = Rot2D(qxz, 1.5f * a);
        qxz = Rot2D(qxz, -pi * std::floor(std::atan2(qxz.y, qxz.x) / pi + 0.5f));
        qxz.x -= 1.0f;
        
        float d = length(PrBox2Df(qxz, vec2(0.2f))) - 0.05f;
        dMin = std::min(dMin, d);
        
        return 0.4f * dMin;
    }
}

inline float Trefoil(const vec3& p_in, float /*time*/, uint32_t /*seed*/) {
    vec3 p = rotationMatrix(vec3(1.0f, 0.0f, 0.0f), pi / 2.0f) * p_in;
    const float scale = 0.18f;
    return detail::ObjDf(p * (1.0f / scale), 3.5f) * scale;
}

} // namespace sdf::geometry


