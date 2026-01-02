#pragma once
// Ported from Dinosaur.glsl
// Copyright 2015 Inigo Quilez @iq - CC BY-NC-SA 3.0

#include "../common.hpp"

namespace sdf {
namespace animal {

namespace dinosaur_detail {
    inline vec2 hash2(float n) {
        return fract(sin(vec2(n, n + 1.0f)) * vec2(13.5453123f, 31.1459123f));
    }
    
    inline vec2 sdSegment(const vec3& p, const vec3& a, const vec3& b) {
        vec3 pa = p - a, ba = b - a;
        float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0f, 1.0f);
        return vec2(length(pa - ba * h), h);
    }
    
    inline float sdSphere(const vec3& p, const vec3& c, float r) {
        return length(p - c) - r;
    }
    
    inline float sdEllipsoid(const vec3& p, const vec3& c, const vec3& r) {
        return (length((p - c) / r) - 1.0f) * std::min(std::min(r.x, r.y), r.z);
    }
    
    inline float det(const vec2& a, const vec2& b) { return a.x * b.y - b.x * a.y; }
    
    inline vec3 getClosest(const vec2& b0, const vec2& b1, const vec2& b2) {
        float a = det(b0, b2);
        float b = 2.0f * det(b1, b0);
        float d = 2.0f * det(b2, b1);
        float f = b * d - a * a;
        vec2 d21 = b2 - b1;
        vec2 d10 = b1 - b0;
        vec2 d20 = b2 - b0;
        vec2 gf = 2.0f * (b * d21 + d * d10 + a * d20);
        gf = vec2(gf.y, -gf.x);
        vec2 pp = -f * gf / dot(gf, gf);
        vec2 d0p = b0 - pp;
        float ap = det(d0p, d20);
        float bp = 2.0f * det(d10, d0p);
        float t = clamp((ap + bp) / (2.0f * a + b + d), 0.0f, 1.0f);
        return vec3(mix(mix(b0, b1, t), mix(b1, b2, t), t), t);
    }
    
    inline vec2 sdBezier(const vec3& a, const vec3& b, const vec3& c, const vec3& p) {
        vec3 w = normalize(cross(c - b, a - b));
        vec3 u = normalize(c - b);
        vec3 v = normalize(cross(w, u));
        
        vec2 a2 = vec2(dot(a - b, u), dot(a - b, v));
        vec2 b2 = vec2(0.0f);
        vec2 c2 = vec2(dot(c - b, u), dot(c - b, v));
        vec3 p3 = vec3(dot(p - b, u), dot(p - b, v), dot(p - b, w));
        
        vec3 cp = getClosest(a2 - vec2(p3.x, p3.y), b2 - vec2(p3.x, p3.y), c2 - vec2(p3.x, p3.y));
        
        return vec2(std::sqrt(dot(vec2(cp.x, cp.y), vec2(cp.x, cp.y)) + p3.z * p3.z), cp.z);
    }
    
    inline vec2 sdLine(const vec3& p, const vec3& a, const vec3& b) {
        vec3 pa = p - a, ba = b - a;
        float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0f, 1.0f);
        return vec2(length(pa - ba * h), h);
    }
    
    inline mat3 base(const vec3& ww) {
        vec3 vv = vec3(0.0f, 0.0f, 1.0f);
        vec3 uu = normalize(cross(vv, ww));
        return mat3(uu.x, ww.x, vv.x, uu.y, ww.y, vv.y, uu.z, ww.z, vv.z);
    }
    
    inline vec2 leg(const vec3& p, const vec3& pa, const vec3& pb, const vec3& pc, float m, float h) {
        float l = sign(pa.z);
        
        vec2 b = sdLine(p, pa, pb);
        float tr = 0.15f;
        float d3 = b.x - tr;
        
        b = sdLine(p, pb, pc);
        tr = 0.15f;
        d3 = smin(d3, b.x - tr, 0.1f);
        
        // knee
        float d4 = sdEllipsoid(p, pb + vec3(-0.02f, 0.05f, 0.0f), vec3(0.14f));
        d4 -= 0.015f * std::abs(std::sin(40.0f * p.y));
        d3 = smin(d3, d4, 0.05f);
        
        // paw
        vec3 ww = normalize(mix(normalize(pc - pb), vec3(0.0f, 1.0f, 0.0f), h));
        mat3 pr = base(ww);
        vec3 fc = pr * (p - pc) - vec3(0.2f, 0.0f, 0.0f) * (-1.0f + 2.0f * h);
        d4 = sdEllipsoid(fc, vec3(0.0f), vec3(0.4f, 0.25f, 0.4f));
        
        // nails
        float d6 = sdEllipsoid(fc, vec3(0.32f, -0.06f, 0.0f) * (-1.0f + 2.0f * h), 0.95f * vec3(0.1f, 0.2f, 0.15f));
        d6 = std::min(d6, sdEllipsoid(vec3(fc.x, fc.y, std::abs(fc.z)),
            vec3(0.21f * (-1.0f + 2.0f * h), -0.08f * (-1.0f + 2.0f * h), 0.26f),
            0.95f * vec3(0.1f, 0.2f, 0.15f)));
        
        d4 = smax(d4, -d6, 0.03f);
        
        float d5 = sdEllipsoid(fc, vec3(0.0f, 1.85f * (-1.0f + 2.0f * h), 0.0f), vec3(2.0f));
        d4 = smax(d4, d5, 0.03f);
        d6 = smax(d6, d5, 0.03f);
        d5 = sdEllipsoid(fc, vec3(0.0f, -0.75f * (-1.0f + 2.0f * h), 0.0f), vec3(1.0f));
        d4 = smax(d4, d5, 0.03f);
        d6 = smax(d6, d5, 0.03f);
        
        d3 = smin(d3, d4, 0.1f);
        
        // muslo
        d4 = sdEllipsoid(p, pa + vec3(0.0f, 0.2f, -0.1f * l), vec3(0.35f) * m);
        d3 = smin(d3, d4, 0.1f);
        
        return vec2(d3, d6);
    }
    
    inline float mapArlo(vec3 p) {
        // body
        vec3 q = p;
        float co = std::cos(0.2f);
        float si = std::sin(0.2f);
        q.x = co * p.x - si * p.y;
        q.y = si * p.x + co * p.y;
        float d1 = sdEllipsoid(q, vec3(0.0f), vec3(1.3f, 0.75f, 0.8f));
        float d2 = sdEllipsoid(q, vec3(0.05f, 0.45f, 0.0f), vec3(0.8f, 0.6f, 0.5f));
        float d = smin(d1, d2, 0.4f);
        
        // neck wrinkles
        float r = length(p - vec3(-1.2f, 0.2f, 0.0f));
        d -= 0.05f * std::abs(std::sin(35.0f * r)) * std::exp(-7.0f * std::abs(r)) * 
             clamp(1.0f - (p.y - 0.3f) * 10.0f, 0.0f, 1.0f);
        
        // tail
        {
            vec2 b = sdBezier(vec3(1.0f, -0.4f, 0.0f), vec3(2.0f, -0.96f, -0.5f), vec3(3.0f, -0.5f, 1.5f), p);
            float tr = 0.3f - 0.25f * b.y;
            float d3 = b.x - tr;
            d = smin(d, d3, 0.2f);
        }
        
        // neck
        {
            vec2 b = sdBezier(vec3(-0.9f, 0.3f, 0.0f), vec3(-2.2f, 0.5f, 0.0f), vec3(-2.6f, 1.7f, 0.0f), p);
            float tr = 0.35f - 0.23f * b.y;
            float d3 = b.x - tr;
            d = smin(d, d3, 0.15f);
        }
        
        float dn;
        // front-left leg
        {
            vec2 d3 = leg(p, vec3(-0.8f, -0.1f, 0.5f), vec3(-1.5f, -0.5f, 0.65f), vec3(-1.9f, -1.1f, 0.65f), 1.0f, 0.0f);
            d = smin(d, d3.x, 0.2f);
            dn = d3.y;
        }
        // back-left leg
        {
            vec2 d3 = leg(p, vec3(0.5f, -0.4f, 0.6f), vec3(0.3f, -1.05f, 0.6f), vec3(0.8f, -1.6f, 0.6f), 0.5f, 1.0f);
            d = smin(d, d3.x, 0.2f);
            dn = std::min(dn, d3.y);
        }
        // front-right leg
        {
            vec2 d3 = leg(p, vec3(-0.8f, -0.2f, -0.5f), vec3(-1.0f, -0.9f, -0.65f), vec3(-0.7f, -1.6f, -0.65f), 1.0f, 1.0f);
            d = smin(d, d3.x, 0.2f);
            dn = std::min(dn, d3.y);
        }
        // back-right leg
        {
            vec2 d3 = leg(p, vec3(0.5f, -0.4f, -0.6f), vec3(0.8f, -0.9f, -0.6f), vec3(1.6f, -1.1f, -0.7f), 0.5f, 0.0f);
            d = smin(d, d3.x, 0.2f);
            dn = std::min(dn, d3.y);
        }
        
        // head
        vec3 s = vec3(p.x, p.y, std::abs(p.z));
        {
            vec2 l = sdLine(p, vec3(-2.7f, 2.36f, 0.0f), vec3(-2.6f, 1.7f, 0.0f));
            float d3 = l.x - (0.22f - 0.1f * smoothstep(0.1f, 1.0f, l.y));
            
            // mouth
            vec3 mp = p - vec3(-2.7f, 2.16f, 0.0f);
            l = sdLine(mp * vec3(1.0f, 1.0f, 1.0f - 0.2f * std::abs(mp.x) / 0.65f), vec3(0.0f),
                vec3(-3.35f, 2.12f, 0.0f) - vec3(-2.7f, 2.16f, 0.0f));
            
            float d4 = l.x - (0.12f + 0.04f * smoothstep(0.0f, 1.0f, l.y));
            float d5 = sdEllipsoid(s, vec3(-3.4f, 2.5f, 0.0f), vec3(0.8f, 0.5f, 2.0f));
            d4 = smax(d4, d5, 0.03f);
            
            d3 = smin(d3, d4, 0.1f);
            
            // mouth bottom
            {
                vec2 b = sdBezier(vec3(-2.6f, 1.75f, 0.0f), vec3(-2.7f, 2.2f, 0.0f), vec3(-3.25f, 2.12f, 0.0f), p);
                float tr = 0.11f + 0.02f * b.y;
                d4 = b.x - tr;
                d3 = smin(d3, d4, 0.001f + 0.06f * (1.0f - b.y * b.y));
            }
            
            // brows
            vec2 b = sdBezier(vec3(-2.84f, 2.50f, 0.04f), vec3(-2.81f, 2.52f, 0.15f),
                vec3(-2.76f, 2.4f, 0.18f), s + vec3(0.0f, -0.02f, 0.0f));
            float tr = 0.035f - 0.025f * b.y;
            d4 = b.x - tr;
            d3 = smin(d3, d4, 0.025f);
            
            // eye wholes
            d4 = sdEllipsoid(s, vec3(-2.79f, 2.36f, 0.04f), vec3(0.12f, 0.15f, 0.15f));
            d3 = smax(d3, -d4, 0.025f);
            
            // nose holes
            d4 = sdEllipsoid(s, vec3(-3.4f, 2.17f, 0.09f), vec3(0.1f, 0.025f, 0.025f));
            d3 = smax(d3, -d4, 0.04f);
            
            d = smin(d, d3, 0.01f);
        }
        
        // eyes
        float d4 = sdSphere(s, vec3(-2.755f, 2.36f, 0.045f), 0.16f);
        
        d = std::min(d, std::min(dn, d4));
        
        return d;
    }
}

inline float Dinosaur(const vec3& p, float /*time*/, uint32_t /*seed*/) {
    vec3 pRot = rotationMatrix(vec3(0.0f, 1.0f, 0.0f), -pi / 2.0f) * p;
    const float scale = 0.25f;
    return dinosaur_detail::mapArlo(pRot * (1.0f / scale)) * scale;
}

} // namespace animal
} // namespace sdf

