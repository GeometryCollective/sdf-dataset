#pragma once
// Ported from Hexprism.glsl
// Copyright 2013 Inigo Quilez @iq - MIT License

#include "../common.hpp"

namespace sdf::geometry {

namespace detail {
    inline float sdHexPrism(vec3 p, const vec2& h) {
        const vec3 k = vec3(-0.8660254f, 0.5f, 0.57735f);
        p = abs(p);
        vec2 pxy = vec2(p.x, p.y);
        pxy -= 2.0f * std::min(dot(vec2(k.x, k.y), pxy), 0.0f) * vec2(k.x, k.y);
        p.x = pxy.x;
        p.y = pxy.y;
        vec2 d = vec2(
            length(pxy - vec2(clamp(p.x, -k.z * h.x, k.z * h.x), h.x)) * sign(p.y - h.x),
            p.z - h.y
        );
        return std::min(std::max(d.x, d.y), 0.0f) + length(max(d, vec2(0.0f)));
    }
}

inline float Hexprism(const vec3& p, float /*time*/, uint32_t /*seed*/) {
    return detail::sdHexPrism(p, vec2(0.5f));
}

} // namespace sdf::geometry


