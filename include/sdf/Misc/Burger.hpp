#pragma once
// Ported from Burger.glsl
// Copyright 2019 Xor @XorDev - CC BY-NC-SA 3.0

#include "../common.hpp"

namespace sdf::misc {

namespace detail {
    inline float soft(float a, float b, float n) {
        return std::log(std::exp(a * n) + std::exp(b * n)) / n;
    }
    
    inline vec3 hash(const vec3& p) {
        mat3 m = mat3(
            45.32f, 32.42f, -41.55f,
            65.35f, -43.42f, 51.55f,
            45.32f, 29.82f, -41.45f
        );
        return fract(sin(m * p) * vec3(142.83f, 253.36f, 188.64f));
    }
    
    inline vec3 value(const vec3& p) {
        vec3 f = floor(p);
        vec3 s = p - f;
        s *= s * (vec3(3.0f) - s - s);
        
        vec3 h000 = hash(f);
        vec3 h100 = hash(f + vec3(1, 0, 0));
        vec3 h010 = hash(f + vec3(0, 1, 0));
        vec3 h110 = hash(f + vec3(1, 1, 0));
        vec3 h001 = hash(f + vec3(0, 0, 1));
        vec3 h101 = hash(f + vec3(1, 0, 1));
        vec3 h011 = hash(f + vec3(0, 1, 1));
        vec3 h111 = hash(f + vec3(1, 1, 1));
        
        return mix(mix(mix(h000, h100, s.x), mix(h010, h110, s.x), s.y),
                   mix(mix(h001, h101, s.x), mix(h011, h111, s.x), s.y), s.z);
    }
    
    inline float worley(const vec3& p) {
        float d = 1.0f;
        for (int x = -1; x <= 1; x++) {
            for (int y = -1; y <= 1; y++) {
                for (int z = -1; z <= 1; z++) {
                    vec3 f = floor(p + vec3(float(x), float(y), float(z)));
                    vec3 v = p - f - hash(f);
                    d = soft(d, dot(v, v), -6.0f);
                }
            }
        }
        return d;
    }
    
    inline float seed(const vec3& p) {
        float d = 1.0f;
        for (int x = -1; x <= 1; x++) {
            for (int y = -1; y <= 1; y++) {
                vec3 f = floor(vec3(p.x, p.y, 0) + vec3(float(x), float(y), 0));
                vec3 h = hash(f) * vec3(1, 1, 63);
                float cz = std::cos(h.z);
                float sz = std::sin(h.z);
                mat3 m = mat3(cz, sz, 0, sz, -cz, 0, 0, 0, 1);
                vec3 v = m * (p - f - h * 0.9f) * vec3(1.7f, 1.0f, 0.0f);
                d = std::min(d, dot(v, v) + step(9.0f, length(f + 0.6f)) + step(p.z, 2.0f));
            }
        }
        return std::max(0.05f - d, 0.0f);
    }
    
    inline float cheese(vec3 p) {
        p.z += -0.27f + 0.03f * p.x * p.x + 0.1f * soft(dot(vec2(p.x, p.y), vec2(p.x, p.y)) - 3.5f, 0.0f, 10.0f);
        return (length(max(abs(p) - vec3(1.6f, 1.6f, 0.0f), vec3(0.0f))) - 0.02f) * 0.8f;
    }
    
    inline float model(const vec3& p) {
        float d = length(p) - 2.5f;
        float m = soft(length(vec2(p.x, p.y)) - 3.0f,
                      std::pow(p.z - soft(d, 0.0f, 20.0f) * 0.7f + 1.1f, 2.0f) - 0.01f, 10.0f);
        
        if (d < 0.1f) {
            vec3 c = vec3(p.x, p.y, std::max(p.z - 0.35f, 0.0f) * 1.6f);
            float b = soft(length(c + 0.05f * sin(vec3(c.y, c.z, c.x) * 2.0f)) * 0.6f - 1.15f,
                          0.41f - std::abs(p.z + 0.15f) - 0.02f * c.x * c.x, 40.0f);
            m = std::min(m, soft(b, -1.0f - p.z, 20.0f));
            m = std::min(m, soft(length(vec2(p.x, p.y) + 0.1f * sin(vec2(c.y, c.x) * 2.0f)) - 2.1f,
                               std::pow(p.z - 0.03f + 0.03f * p.x * p.x, 2.0f) - 0.04f, 12.0f));
            m = std::min(m, soft(length(p) - 1.9f, std::abs(p.z + 0.4f - 0.03f * p.y * p.y) - 0.1f, 80.0f));
            m = std::min(m, cheese(p));
            
            vec3 r = value(p / dot(p, p) * vec3(14, 14, 1)) - vec3(0.5f);
            vec3 l = p + vec3(0, 0, 0.46f) + vec3(0, 0, length(vec2(p.x, p.y)) - 1.8f) * 0.3f * std::cos(r.x * 5.0f - r.y * 5.0f);
            m = std::min(m, soft(length(l) - 2.1f - 0.4f * r.z, std::abs(l.z) - 0.02f, 28.0f) * 0.8f);
            
            float s = 0.2f * seed(p * 5.0f);
            return m - s;
        }
        return std::min(d, m);
    }
}

inline float Burger(const vec3& p, float /*time*/, uint32_t /*seed*/) {
    const float scale = 0.3f;
    vec3 pRot = rotationMatrix(vec3(1.0f, 0.0f, 0.0f), pi / 2.0f) * (p * (1.0f / scale));
    return detail::model(pRot) * scale;
}

} // namespace sdf::misc


