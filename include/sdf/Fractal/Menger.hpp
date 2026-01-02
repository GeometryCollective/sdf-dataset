#pragma once
// Ported from Menger.glsl
// Copyright 2013 Inigo Quilez @iq - MIT License

#include "../common.hpp"

namespace sdf::fractal {

namespace detail {
    inline float maxcomp(const vec3& p) { 
        return std::max(p.x, std::max(p.y, p.z)); 
    }
    
    inline float sdBox(const vec3& p, const vec3& b) {
        vec3 di = abs(p) - b;
        float mc = maxcomp(di);
        return std::min(mc, length(max(di, vec3(0.0f))));
    }
}

inline float Menger(const vec3& p_in, float /*time*/, uint32_t /*seed*/) {
    vec3 p = p_in;
    float d = detail::sdBox(p, vec3(1.0f));
    
    float s = 1.0f;
    for (int m = 0; m < 7; ++m) {
        vec3 a = mod(p * s, 2.0f) - 1.0f;
        s *= 3.0f;
        vec3 r = abs(vec3(1.0f) - 3.0f * abs(a));
        
        float da = std::max(r.x, r.y);
        float db = std::max(r.y, r.z);
        float dc = std::max(r.z, r.x);
        float c = (std::min(da, std::min(db, dc)) - 1.0f) / s;
        
        d = std::max(d, c);
    }
    
    return d;
}

} // namespace sdf::fractal


