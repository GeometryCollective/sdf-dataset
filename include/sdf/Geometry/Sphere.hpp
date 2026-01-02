#pragma once
// Ported from Sphere.glsl
// Copyright 2013 Inigo Quilez @iq - MIT License

#include "../common.hpp"

namespace sdf::geometry {

namespace detail {
    inline float sdSphere(const vec3& p, float s) {
        return length(p) - s;
    }
}

inline float Sphere(const vec3& p, float /*time*/, uint32_t /*seed*/) {
    return detail::sdSphere(p, 0.5f);
}

} // namespace sdf::geometry


