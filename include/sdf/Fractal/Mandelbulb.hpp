#pragma once
// Ported from Mandelbulb.glsl
// Copyright 2014 Morgan McGuire @CasualEffects - BSD License

#include "../common.hpp"

namespace sdf::fractal {

namespace detail {
    constexpr float minimumDistanceToSurface = 0.0003f;
    constexpr int ITERATIONS = 16;
    constexpr float power = 8.0f;
    
    inline float distanceToSurface(const vec3& P, float& AO) {
        AO = 1.0f;
        
        vec3 Q = P;
        
        // Bounding sphere check
        {
            const float externalBoundingRadius = 1.2f;
            float r = length(P) - externalBoundingRadius;
            if (r > 1.0f) { return r; }
        }
        
        const float internalBoundingRadius = 0.72f;
        float derivative = 1.0f;
        
        for (int i = 0; i < ITERATIONS; ++i) {
            AO *= 0.725f;
            float r = length(Q);
            
            if (r > 2.0f) {
                AO = std::min((AO + 0.075f) * 4.1f, 1.0f);
                return std::min(length(P) - internalBoundingRadius, 0.5f * std::log(r) * r / derivative);
            } else {
                float theta = std::acos(Q.z / r) * power;
                float phi = std::atan2(Q.y, Q.x) * power;
                
                derivative = std::pow(r, power - 1.0f) * power * derivative + 1.0f;
                
                float sinTheta = std::sin(theta);
                
                Q = vec3(sinTheta * std::cos(phi),
                        sinTheta * std::sin(phi),
                        std::cos(theta)) * std::pow(r, power) + P;
            }
        }
        
        return minimumDistanceToSurface;
    }
}

inline float Mandelbulb(const vec3& p, float /*time*/, uint32_t /*seed*/) {
    const float scale = 0.6f;
    float ignore;
    return detail::distanceToSurface(p * (1.0f / scale), ignore) * scale;
}

} // namespace sdf::fractal


