#pragma once
// Ported from Capsule.glsl
// Copyright 2013 Inigo Quilez @iq - MIT License

#include "../common.hpp"

namespace sdf::geometry {

namespace detail {
    inline float sdVerticalCapsule(vec3 p, float h, float r) {
        p.y -= clamp(p.y, 0.0f, h);
        return length(p) - r;
    }
}

inline float Capsule(const vec3& p, float /*time*/, uint32_t /*seed*/) {
    return detail::sdVerticalCapsule(p + vec3(0.0f, 0.5f, 0.0f), 1.0f, 0.5f);
}

} // namespace sdf::geometry


