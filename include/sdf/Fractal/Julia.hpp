#pragma once
// Ported from Julia.glsl
// Copyright 2013 Inigo Quilez @iq - CC BY-NC-SA 3.0

#include "../common.hpp"

namespace sdf::fractal {

namespace detail {
    inline float lengthSquared(const vec4& v) {
        return dot(v, v);
    }
    
    inline vec4 qSquare(const vec4& a) {
        return vec4(a.x * a.x - dot(vec3(a.y, a.z, a.w), vec3(a.y, a.z, a.w)), 
                    2.0f * a.x * vec3(a.y, a.z, a.w));
    }
    
    inline vec4 qCube(const vec4& a) {
        return a * (4.0f * a.x * a.x - dot(a, a) * vec4(3.0f, 1.0f, 1.0f, 1.0f));
    }
    
    inline vec3 julia_map(const vec3& p, const vec4& c) {
        vec4 z = vec4(p, 0.2f);
        
        float m2 = 0.0f;
        vec2 t = vec2(1e10f);
        
        float dz2 = 1.0f;
        for (int i = 0; i < 10; i++) {
            dz2 *= 9.0f * lengthSquared(qSquare(z));
            z = qCube(z) + c;
            
            m2 = dot(z, z);
            if (m2 > 10000.0f) break;
            
            t = min(t, vec2(m2, std::abs(z.x)));
        }
        
        float d = 0.25f * std::log(m2) * std::sqrt(m2 / dz2);
        return vec3(d, t);
    }
}

inline float Julia(const vec3& p, float /*time*/, uint32_t /*seed*/) {
    const float t = 10.0f;
    const vec4 c = vec4(-0.1f, 0.6f, 0.9f, -0.3f) + 
                   0.1f * sin(vec4(3.0f, 0.0f, 1.0f, 2.0f) + 0.5f * vec4(1.0f, 1.3f, 1.7f, 2.1f) * t);
    const float scale = 0.8f;
    return detail::julia_map(p * (1.0f / scale), c).x * scale;
}

} // namespace sdf::fractal


