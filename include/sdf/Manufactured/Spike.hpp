#pragma once
// Ported from Spike.glsl
// Copyright 2013 @Patapom - CC BY-NC-SA 3.0

#include "../common.hpp"

namespace sdf {
namespace manufactured {

namespace spike_detail {
    inline float spikeball(const vec3& p) {
        const vec3 c[19] = {
            vec3(1.0f, 0.0f, 0.0f),
            vec3(0.0f, 1.0f, 0.0f),
            vec3(0.0f, 0.0f, 1.0f),
            vec3(0.577f, 0.577f, 0.577f),
            vec3(-0.577f, 0.577f, 0.577f),
            vec3(0.577f, -0.577f, 0.577f),
            vec3(0.577f, 0.577f, -0.577f),
            vec3(0.0f, 0.357f, 0.934f),
            vec3(0.0f, -0.357f, 0.934f),
            vec3(0.934f, 0.0f, 0.357f),
            vec3(-0.934f, 0.0f, 0.357f),
            vec3(0.357f, 0.934f, 0.0f),
            vec3(-0.357f, 0.934f, 0.0f),
            vec3(0.0f, 0.851f, 0.526f),
            vec3(0.0f, -0.851f, 0.526f),
            vec3(0.526f, 0.0f, 0.851f),
            vec3(-0.526f, 0.0f, 0.851f),
            vec3(0.851f, 0.526f, 0.0f),
            vec3(-0.851f, 0.526f, 0.0f)
        };
        
        float MinDistance = 1e4f;
        for (int i = 3; i < 19; i++) {
            float d = clamp(dot(p, c[i]), -1.0f, 1.0f);
            vec3 proj = d * c[i];
            d = std::abs(d);
            
            float Distance2Spike = length(p - proj);
            float SpikeThickness = 0.25f * std::exp(-5.0f * d) + 0.0f;
            float Distance = Distance2Spike - SpikeThickness;
            
            MinDistance = std::min(MinDistance, Distance);
        }
        
        return MinDistance;
    }
}

inline float Spike(const vec3& p, float /*time*/, uint32_t /*seed*/) {
    return spike_detail::spikeball(p);
}

} // namespace manufactured
} // namespace sdf

