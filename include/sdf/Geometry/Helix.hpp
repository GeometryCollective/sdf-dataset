#pragma once
// Ported from Helix.glsl
// Copyright 2016 Xor @XorDev - CC BY-NC-SA 3.0

#include "../common.hpp"

namespace sdf::geometry {

namespace helix_detail {
    inline float sdBox(const vec3& p, const vec3& b) {
        vec3 q = abs(p) - b;
        return length(max(q, vec3(0.0f))) + std::min(std::max(q.x, std::max(q.y, q.z)), 0.0f);
    }
    
    inline float model(const vec3& p) {
        const float FULL_SIZE = 2.0f;
        const float EDGE_SIZE = 0.2f;
        const float PAIR_SIZE = 0.2f;
        
        float A = p.z / 3.0f;
        vec3 R = vec3(std::cos(A), std::sin(A), 0.0f);
        vec3 C = vec3(p.x + R.y, p.y - R.x, fract(p.z) - 0.5f);
        
        float H = std::min(length(vec2(C.x, C.y) + vec2(R.x, R.y) * FULL_SIZE), 
                          length(vec2(C.x, C.y) - vec2(R.x, R.y) * FULL_SIZE)) * 0.5f - EDGE_SIZE;
        float P = std::max(length(vec2(dot(vec2(C.x, C.y), vec2(R.y, -R.x)), C.z)) - PAIR_SIZE,
                          length(vec2(C.x, C.y)) - FULL_SIZE);
        
        return std::min(H, P);
    }
} // namespace helix_detail

inline float Helix(const vec3& p, float /*time*/, uint32_t /*seed*/) {
    float boxD = helix_detail::sdBox(p, vec3(1.0f, 1.0f, 1.0f));
    const float scale = 0.125f;
    vec3 pScaled = p * (1.0f / scale);
    vec3 pRot = rotationMatrix(vec3(1.0f, 0.0f, 0.0f), pi / 2.0f) * pScaled;
    return std::max(boxD, helix_detail::model(pRot)) * scale;
}

} // namespace sdf::geometry


