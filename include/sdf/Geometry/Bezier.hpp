#pragma once
// Ported from Bezier.glsl
// Copyright 2013 Inigo Quilez @iq - MIT License

#include "../common.hpp"

namespace sdf::geometry {

namespace bezier_detail {
    inline float dot2(const vec3& v) { return dot(v, v); }
    
    inline float sdBezier(const vec3& pos, const vec3& A, const vec3& B, const vec3& C) {
        vec3 a = B - A;
        vec3 b = A - 2.0f * B + C;
        vec3 c = a * 2.0f;
        vec3 d = A - pos;
        float kk = 1.0f / dot(b, b);
        float kx = kk * dot(a, b);
        float ky = kk * (2.0f * dot(a, a) + dot(d, b)) / 3.0f;
        float kz = kk * dot(d, a);
        float res = 0.0f;
        float p = ky - kx * kx;
        float p3 = p * p * p;
        float q = kx * (2.0f * kx * kx - 3.0f * ky) + kz;
        float h = q * q + 4.0f * p3;
        
        if (h >= 0.0f) {
            h = std::sqrt(h);
            vec3 x = (vec3(h, -h, h) - q) / 2.0f;
            vec3 uv = sign(x) * pow(abs(x), vec3(1.0f / 3.0f));
            float t = clamp(uv.x + uv.y - kx, 0.0f, 1.0f);
            res = dot2(d + (c + b * t) * t);
        } else {
            float z = std::sqrt(-p);
            float v = std::acos(q / (p * z * 2.0f)) / 3.0f;
            float m = std::cos(v);
            float n = std::sin(v) * 1.732050808f;
            vec3 t = clamp(vec3(m + m, -n - m, n - m) * z - kx, 0.0f, 1.0f);
            res = std::min(dot2(d + (c + b * t.x) * t.x),
                          dot2(d + (c + b * t.y) * t.y));
        }
        return std::sqrt(res);
    }
} // namespace bezier_detail

inline float Bezier(const vec3& p, float /*time*/, uint32_t /*seed*/) {
    float radius = 0.01f;
    return bezier_detail::sdBezier(p, vec3(0.0f, 0.75f, 0.0f), vec3(0.5f, 0.8f, 0.0f), vec3(-0.75f, -0.65f, 0.0f)) - radius;
}

} // namespace sdf::geometry


