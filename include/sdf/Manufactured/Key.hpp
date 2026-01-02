#pragma once
// Ported from Key.glsl
// Copyright 2020 Flopine @Flopine - CC BY-NC-SA 3.0

#include "../common.hpp"

namespace sdf {
namespace manufactured {

namespace key_detail {
    inline float cyl(const vec3& p, float r, float h) {
        return std::max(length(vec2(p.x, p.y)) - r, std::abs(p.z) - h);
    }
    
    inline float tore(const vec3& p, const vec2& t) {
        return length(vec2(length(vec2(p.x, p.z)) - t.x, p.y)) - t.y;
    }
    
    inline float key(vec3 p, float t) {
        float thick = t;
        float body = cyl(vec3(p.x, p.z, p.y), thick, 1.5f);
        float encoche = tore(vec3(p.x, p.z, p.y) + vec3(-(2.0f * thick), 0.05f, 1.0f), vec2(thick, 0.1f));
        float head = std::max(-cyl(p - vec3(0.0f, 2.2f, 0.0f), 0.65f, thick * 1.5f),
                              cyl(p - vec3(0.0f, 2.2f, 0.0f), 0.8f, thick));
        p.y = std::abs(std::abs(p.y - 0.45f) - 0.8f) - 0.15f;
        float ts = tore(p, vec2(thick, 0.08f));
        
        return std::min(encoche, std::min(std::min(body, head), ts));
    }
    
    inline float SDF(vec3 p) {
        vec3 pp = p - vec3(0.0f, 2.0f, 0.0f);
        float small = 3.5f;
        float thick = 0.25f;
        float d = key(p, thick);
        for (int i = 0; i < 2; i++) {
            d = std::min(d, key(pp * small, thick) / small);
            pp.y -= 0.55f;
            small *= 4.0f;
        }
        return d;
    }
}

inline float Key(const vec3& p, float /*time*/, uint32_t /*seed*/) {
    vec3 pTrans = p + vec3(0.0f, 0.2f, 0.0f);
    const float scale = 0.3f;
    return key_detail::SDF(pTrans * (1.0f / scale)) * scale;
}

} // namespace manufactured
} // namespace sdf

