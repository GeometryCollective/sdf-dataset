#pragma once
// Ported from Serpinski.glsl
// Copyright 2014 al13n @al13n - CC BY-NC-SA 3.0

#include "../common.hpp"

namespace sdf::fractal {

inline float Serpinski(const vec3& p_in, float /*time*/, uint32_t /*seed*/) {
    vec3 p = p_in * 2.5f;
    const vec3 p0 = vec3(-1, -1, -1);
    const vec3 p1 = vec3(1, 1, -1);
    const vec3 p2 = vec3(1, -1, 1);
    const vec3 p3 = vec3(-1, 1, 1);
    
    const int maxit = 25;
    const float scale = 2.0f;
    const float minSize = std::pow(scale, -float(maxit - 2));
    
    for (int i = 0; i < maxit; ++i) {
        float d = distance(p, p0);
        vec3 c = p0;
        
        float t = distance(p, p1);
        if (t < d) { d = t; c = p1; }
        
        t = distance(p, p2);
        if (t < d) { d = t; c = p2; }
        
        t = distance(p, p3);
        if (t < d) { d = t; c = p3; }
        
        p = (p - c) * scale;
    }
    
    return (1.0f / 2.5f) * (length(p) * std::pow(scale, float(-maxit)) - minSize);
}

} // namespace sdf::fractal


