#pragma once
// Ported from Rock.glsl
// Copyright 2014 Alexander Alekseev @TDM - CC BY-NC-SA 3.0

#include "../common.hpp"

namespace sdf {
namespace nature {

namespace rock_detail {
    inline float hash11(float p) {
        return fract(std::sin(p * 727.1f) * 435.545f);
    }
    
    inline float hash12(const vec2& p) {
        float h = dot(p, vec2(127.1f, 311.7f));
        return fract(std::sin(h) * 437.545f);
    }
    
    inline vec3 hash31(float p) {
        vec3 h = vec3(127.231f, 491.7f, 718.423f) * p;
        return fract(sin(h) * 435.543f);
    }
    
    inline float noise_3(const vec3& p) {
        vec3 i = floor(p);
        vec3 f = fract(p);
        vec3 u = f * f * (vec3(3.0f) - 2.0f * f);
        
        vec2 ii = vec2(i.x, i.y) + i.z * vec2(5.0f);
        float a = hash12(ii + vec2(0.0f, 0.0f));
        float b = hash12(ii + vec2(1.0f, 0.0f));
        float c = hash12(ii + vec2(0.0f, 1.0f));
        float d = hash12(ii + vec2(1.0f, 1.0f));
        float v1 = mix(mix(a, b, u.x), mix(c, d, u.x), u.y);
        
        ii += vec2(5.0f);
        a = hash12(ii + vec2(0.0f, 0.0f));
        b = hash12(ii + vec2(1.0f, 0.0f));
        c = hash12(ii + vec2(0.0f, 1.0f));
        d = hash12(ii + vec2(1.0f, 1.0f));
        float v2 = mix(mix(a, b, u.x), mix(c, d, u.x), u.y);
        
        return std::max(mix(v1, v2, u.z), 0.0f);
    }
    
    inline float fbm3_high(vec3 p, float a, float f) {
        float ret = 0.0f;
        float amp = 1.0f;
        float frq = 1.0f;
        for (int i = 0; i < 5; i++) {
            float n = std::pow(noise_3(p * frq), 2.0f);
            ret += n * amp;
            frq *= f;
            amp *= a * std::pow(n, 0.2f);
        }
        return ret;
    }
    
    inline float rock_sphere(const vec3& p, float r) {
        return length(p) - r;
    }
    
    inline float boolSub(float a, float b) { return std::max(a, -b); }
    
    inline float rock(const vec3& p) {
        float d = rock_sphere(p, 1.0f);
        for (int i = 0; i < 9; i++) {
            float ii = float(i);
            float r = 2.5f + hash11(ii);
            vec3 v = normalize(hash31(ii) * 2.0f - 1.0f);
            d = boolSub(d, rock_sphere(p + v * r, r * 0.8f));
        }
        return d;
    }
    
    inline float map_detailed(const vec3& p) {
        const float DISPLACEMENT = 0.1f;
        return rock(p) + fbm3_high(p * 4.0f, 0.4f, 2.96f) * DISPLACEMENT;
    }
}

inline float Rock(const vec3& p, float /*time*/, uint32_t /*seed*/) {
    const float scale = 0.8f;
    return rock_detail::map_detailed(p * (1.0f / scale)) * scale * 0.7f;
}

} // namespace nature
} // namespace sdf

