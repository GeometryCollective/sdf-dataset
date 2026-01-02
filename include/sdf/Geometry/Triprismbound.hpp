#pragma once
// Ported from Triprismbound.glsl
// Copyright 2013 Inigo Quilez @iq - MIT License

#include "../common.hpp"

namespace sdf::geometry {

namespace detail {
    inline float sdTriPrism(vec3 p, const vec2& h) {
        vec3 q = abs(p);
        return std::max(q.z - h.y, std::max(q.x * 0.866025f + p.y * 0.5f, -p.y) - h.x * 0.5f);
    }
}

inline float Triprismbound(const vec3& p, float /*time*/, uint32_t /*seed*/) {
    return detail::sdTriPrism(p, vec2(0.5f));
}

} // namespace sdf::geometry


