#pragma once
// Ported from Torus.glsl
// Copyright 2013 Inigo Quilez @iq - MIT License

#include "../common.hpp"

namespace sdf::geometry {

namespace detail {
    inline float sdTorus(const vec3& p, const vec2& t) {
        return length(vec2(length(vec2(p.x, p.z)) - t.x, p.y)) - t.y;
    }
}

inline float Torus(const vec3& p_in, float /*time*/, uint32_t /*seed*/) {
    vec3 p = rotationMatrix(vec3(1.0f, 0.0f, 0.0f), pi / 2.0f) * p_in;
    return detail::sdTorus(p, vec2(0.4f, 0.2f));
}

} // namespace sdf::geometry


