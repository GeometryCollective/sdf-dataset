#pragma once
// Ported from Roundbox.glsl
// Copyright 2013 Inigo Quilez @iq - MIT License

#include "../common.hpp"

namespace sdf::geometry {

namespace detail {
    inline float sdRoundBox(const vec3& p, const vec3& b, float r) {
        vec3 q = abs(p) - b;
        return length(max(q, vec3(0.0f))) + std::min(std::max(q.x, std::max(q.y, q.z)), 0.0f) - r;
    }
}

inline float Roundbox(const vec3& p, float /*time*/, uint32_t /*seed*/) {
    return detail::sdRoundBox(p, vec3(0.4f), 0.25f);
}

} // namespace sdf::geometry


