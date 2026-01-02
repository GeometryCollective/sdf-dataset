#pragma once
// Ported from Dalek.glsl
// Copyright 2013 Antonalog @Antonalog - CC BY-NC-SA 3.0

#include "../common.hpp"

namespace sdf {
namespace misc {

namespace dalek_detail {
    inline float Sphere(const vec3& p, const vec3& c, float r) { return length(p - c) - r; }
    
    inline float Box(const vec3& p, const vec3& b) {
        vec3 d = abs(p) - b;
        return std::min(std::max(d.x, std::max(d.y, d.z)), 0.0f) + length(max(d, vec3(0.0f)));
    }
    
    inline float BevelBox(const vec3& p, const vec3& size, float box_r) {
        vec3 box_edge = size - box_r * 0.5f;
        vec3 dd = abs(p) - box_edge;
        float maxdd = std::max(std::max(dd.x, dd.y), dd.z);
        maxdd = std::min(maxdd, 0.0f);
        dd = max(dd, vec3(0.0f));
        float ddd = length(dd) - box_r;
        ddd += maxdd;
        return ddd;
    }
    
    inline float CylinderXZ(const vec3& p, const vec3& c) { return length(vec2(p.x, p.z) - vec2(c.x, c.y)) - c.z; }
    inline float CylinderXY(const vec3& p, const vec3& c) { return length(vec2(p.x, p.y) - vec2(c.x, c.y)) - c.z; }
    inline float CylinderYZ(const vec3& p, const vec3& c) { return length(vec2(p.y, p.z) - vec2(c.x, c.y)) - c.z; }
    
    inline float udHexPrism(const vec2& p, float h) {
        vec2 q = abs(p);
        return std::max(q.x + q.y * 0.57735f, q.y * 1.1547f) - h;
    }
    
    inline vec3 RotX(const vec3& p, float t) {
        float c = std::cos(t), s = std::sin(t);
        return vec3(p.x, p.y * c + p.z * s, -p.y * s + p.z * c);
    }
    inline vec3 RotY(const vec3& p, float t) {
        float c = std::cos(t), s = std::sin(t);
        return vec3(p.x * c + p.z * s, p.y, -p.x * s + p.z * c);
    }
    inline vec3 RotZ(const vec3& p, float t) {
        float c = std::cos(t), s = std::sin(t);
        return vec3(p.x * c + p.y * s, -p.x * s + p.y * c, p.z);
    }
    
    inline float Plate(vec3 p, float h) {
        p = RotX(p, -pi * 0.0625f);
        float hh = 0.25f, w = 0.5f * hh, bev = 0.02f;
        float base = BevelBox(p - vec3(0.0f, 0.0f, 0.0f), vec3(w, h, w), bev);
        float scallop = BevelBox(RotX(p, -pi * 0.0625f) - vec3(0.0f, -0.6f * h, 0.6f * hh), vec3(w, 2.0f * h, w) * 0.5f, bev);
        base = std::max(base, -scallop);
        float hole_off = h * 0.8f;
        vec3 reflect_y_p = vec3(p.x, std::abs(p.y), p.z);
        float rivet = Sphere(reflect_y_p, vec3(0.0f, hole_off, w), 0.03f);
        base = std::min(base, rivet);
        return base;
    }
    
    inline float PlateRing(const vec3& p, float polar_t, float polar_r) {
        float h = std::abs(polar_t) < pi * (3.0f / 8.0f) ? 0.25f : 0.5f;
        float pt = mod(polar_t, pi * (1.0f / 8.0f)) - pi * (1.0f / 8.0f) * 0.5f;
        vec3 q = vec3(polar_r * std::sin(pt), p.y, polar_r * std::cos(pt));
        q -= vec3(0.0f, -(h - 0.25f), 1.0f);
        return Plate(q, h);
    }
    
    inline float Whisk(vec3 p) {
        p = abs(p);
        float r = 0.075f;
        float c = std::min(0.4f - p.x, 0.1f) * r * 12.0f;
        return length(vec2(p.z, p.y) - vec2(c, c)) - r * 0.25f;
    }
    
    inline float Gun(vec3 p) {
        p -= vec3(1.7f, -0.55f, -0.70f);
        float d = Whisk(p);
        d = std::min(d, Whisk(RotX(p, pi * 0.25f)));
        float barrel = length(vec2(p.z, p.y)) - 0.05f;
        barrel = std::max(barrel, std::abs(p.x) - 0.5f);
        barrel = std::max(barrel, -(length(vec2(p.z, p.y)) - 0.025f));
        return std::min(d, barrel);
    }
    
    const float suck_end = 1.0f;
    
    inline float Plunger(vec3 p) {
        p -= vec3(1.7f, -0.55f, 0.70f);
        float barrel = length(vec2(p.z, p.y)) - 0.075f;
        barrel = std::max(barrel, std::abs(p.x) - 0.75f);
        float sucker = Sphere(p, vec3(suck_end, 0.0f, 0.0f), 0.3f);
        sucker = std::max(sucker, -Sphere(p, vec3(suck_end, 0.0f, 0.0f), 0.25f));
        sucker = std::max(sucker, p.x - 0.9f);
        return std::min(barrel, sucker);
    }
    
    inline float GunPort(vec3 p) {
        p.z = std::abs(p.z);
        float w = 0.225f, d = 0.5f;
        vec3 c = vec3(0.75f - 0.25f, -0.55f, 0.70f);
        float s = Sphere(p, c + vec3(0.35f + 0.25f, 0.0f, 0.0f), w * 0.66f);
        p.x += 0.2f * p.y;
        float b = BevelBox(p - c, vec3(d, w, w), 0.02f);
        return std::min(b, s);
    }
    
    inline float Balls(vec3 p, float polar_t, float polar_r) {
        p.y += 2.45f;
        float ang_reps = 6.0f;
        float pt = mod(polar_t, pi * (1.0f / ang_reps)) - pi * (1.0f / ang_reps) * 0.5f;
        vec3 q = vec3(polar_r * std::sin(pt), p.y, polar_r * std::cos(pt));
        float k = 0.5f;
        q.y = mod(q.y, k) - 0.5f * k;
        float balls = Sphere(q, vec3(0.0f, 0.0f, 1.25f - 0.1f * std::floor(p.y * 2.0f)), 0.2f);
        balls = std::max(balls, std::abs(p.y) - 1.0f);
        return balls;
    }
    
    inline float Body(vec3 p) {
        vec3 q = p;
        p = RotY(p, pi * 1.0f / 12.0f);
        float taper = 1.0f + 0.1f * p.y;
        taper -= p.y < -3.5f ? 0.2f * clamp(-(p.y + 3.5f), 0.0f, 0.1f) : 0.0f;
        p.x *= taper; p.z *= taper;
        float w = 1.15f;
        float d = udHexPrism(vec2(p.z, p.x), w);
        d = std::max(d, udHexPrism(vec2(p.x, p.z), w));
        d /= taper;
        q.y += 2.45f;
        d = std::max(d, std::abs(q.y) - 1.5f);
        return d;
    }
    
    inline float Belt(vec3 p, float polar_t, float polar_r) {
        float r = p.y + 1.05f;
        float d = CylinderXZ(p, vec3(0.0f, 0.0f, 1.25f - 0.15f * r));
        vec3 q = p;
        q.y += 1.05f;
        d = std::max(d, std::abs(q.y) - 0.125f);
        float b = CylinderXZ(p, vec3(0.0f, 0.0f, 0.8f - 0.15f * p.y));
        b = std::max(b, std::abs(p.y) - 1.2f);
        d = std::min(d, BevelBox(p + vec3(-0.8f, 0.60f, 0.0f), vec3(0.2f, 0.2f, 0.4f + 0.2f * p.y), 0.05f));
        return std::min(d, b);
    }
    
    inline float Grill(vec3 p, float polar_t, float polar_r) {
        p += vec3(0.0f, -0.5f, 0.0f);
        vec3 c = p;
        float k = 0.25f;
        c.y = mod(c.y + 0.1f, k) - 0.5f * k;
        float b = CylinderXZ(c, vec3(0.0f, 0.0f, 0.9f));
        b = std::max(b, std::abs(c.y) - 0.025f);
        b = std::max(b, std::abs(p.y) - 0.5f);
        float ang_reps = 4.0f;
        float pt = mod(polar_t, pi * (1.0f / ang_reps)) - pi * (1.0f / ang_reps) * 0.5f;
        vec3 q = vec3(polar_r * std::cos(pt), p.y, polar_r * std::sin(pt));
        q = RotZ(q, pi * 0.06f);
        float d = BevelBox(q, vec3(0.8f, 0.5f, 0.05f), 0.045f);
        return std::min(d, b);
    }
    
    inline float Head(const vec3& p) {
        float d = Sphere(p, vec3(0.0f, 0.66f, 0.0f), 1.0f);
        d = std::max(d, -p.y + 1.0f);
        return d;
    }
    
    inline float Eye(vec3 p) {
        p = RotZ(p, pi * 0.05f);
        float d = CylinderYZ(p, vec3(1.1f, 0.0f, 0.1f));
        d = std::max(d, -p.x);
        d = std::min(d, Sphere(p, vec3(2.4f, 1.1f, 0.0f), 0.2f));
        d = std::max(d, p.x - 2.5f);
        d = std::min(d, Sphere(p, vec3(2.4f, 1.1f, 0.0f), 0.15f));
        d = std::min(d, BevelBox(p + vec3(-0.9f, -1.1f, 0.0f), vec3(0.2f, 0.2f, 0.4f - 0.2f * p.y), 0.05f));
        return d;
    }
    
    inline float Ears(vec3 p) {
        p.z = std::abs(p.z);
        p = RotX(p, -pi * 0.25f);
        float d = CylinderXY(p, vec3(0.0f, 0.5f, 0.2f - 0.1f * (p.z - 0.5f)));
        d = std::max(d, p.z - 1.75f);
        return d;
    }
    
    inline float dalek(vec3 p) {
        float polar_t = std::atan2(p.z, p.x);
        float polar_r = length(vec2(p.x, p.z));
        
        float d = 1e10f;
        float d_bound = 2.5f;
        if (polar_r < d_bound) {
            d = std::min(d, Balls(p, polar_t, polar_r));
            d = std::min(d, Belt(p, polar_t, polar_r));
            d = std::min(d, PlateRing(p, polar_t, polar_r));
            d = std::min(d, Grill(p, polar_t, polar_r));
            d = std::min(d, Head(p));
            d = std::min(d, Ears(p));
        }
        
        d = std::min(d, Body(p));
        if (std::abs(polar_t) < pi * 0.5f) {
            d = std::min(d, Eye(p));
            d = std::min(d, GunPort(p));
            d = std::min(d, Gun(p));
            d = std::min(d, Plunger(p));
        }
        
        return d;
    }
}

inline float Dalek(const vec3& p, float /*time*/, uint32_t /*seed*/) {
    vec3 pRot = rotationMatrix(vec3(0.0f, 1.0f, 0.0f), pi / 2.0f) * (p + vec3(0.0f, -0.2f, 0.0f));
    const float scale = 0.25f;
    return dalek_detail::dalek(pRot * (1.0f / scale)) * scale * 0.5f;
}

} // namespace misc
} // namespace sdf

