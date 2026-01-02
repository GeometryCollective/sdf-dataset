#pragma once
// Ported from Mushroom.glsl
// Copyright 2017 Inigo Quilez @iq - CC BY-NC-SA 3.0

#include "../common.hpp"

namespace sdf {
namespace nature {

namespace mushroom_detail {
    inline float length2(const vec3& p) { return dot(p, p); }
    
    inline float sdEllipsoid(const vec3& pos, const vec3& cen, const vec3& rad) {
        vec3 p = pos - cen;
        float d = length(p / rad) - 1.0f;
        return d * std::min(std::min(rad.x, rad.y), rad.z);
    }
    
    inline vec2 sdLine(const vec3& pos, const vec3& a, const vec3& b) {
        vec3 pa = pos - a;
        vec3 ba = b - a;
        float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0f, 1.0f);
        return vec2(length(pa - h * ba), h);
    }
    
    inline vec3 rotateX(vec3 p, float t) {
        float co = std::cos(t);
        float si = std::sin(t);
        float y = p.y * co + p.z * si;
        float z = -p.y * si + p.z * co;
        return vec3(p.x, y, z);
    }
    
    inline vec3 rotateY(vec3 p, float t) {
        float co = std::cos(t);
        float si = std::sin(t);
        float x = p.x * co + p.z * si;
        float z = -p.x * si + p.z * co;
        return vec3(x, p.y, z);
    }
    
    inline vec3 rotateZ(vec3 p, float t) {
        float co = std::cos(t);
        float si = std::sin(t);
        float x = p.x * co + p.y * si;
        float y = -p.x * si + p.y * co;
        return vec3(x, y, p.z);
    }
    
    inline vec3 mapLadyBug(vec3 p, float curmin) {
        float db = length(p - vec3(0.0f, -0.35f, 0.05f)) - 1.3f;
        if (db > curmin) return vec3(10000.0f, 0.0f, 0.0f);
        
        float dBody = sdEllipsoid(p, vec3(0.0f), vec3(0.8f, 0.75f, 1.0f));
        dBody = smax(dBody, -sdEllipsoid(p, vec3(0.0f, -0.1f, 0.0f), vec3(0.75f, 0.7f, 0.95f)), 0.05f);
        dBody = smax(dBody, -sdEllipsoid(p, vec3(0.0f, 0.0f, 0.8f), vec3(0.35f, 0.35f, 0.5f)), 0.05f);
        dBody = smax(dBody, sdEllipsoid(p, vec3(0.0f, 1.7f, -0.1f), vec3(2.0f)), 0.05f);
        dBody = smax(dBody, -std::abs(p.x) + 0.005f, 0.02f + 0.1f * clamp(p.z * p.z * p.z * p.z, 0.0f, 1.0f));
        
        vec3 res = vec3(dBody, 3.0f, 0.0f);
        
        // Head
        vec3 hc = vec3(0.0f, 0.1f, 0.8f);
        vec3 ph = rotateX(p - hc, 0.5f);
        float dHead = sdEllipsoid(ph, vec3(0.0f), vec3(0.35f, 0.25f, 0.3f));
        dHead = smax(dHead, -sdEllipsoid(ph, vec3(0.0f, -0.95f, 0.0f), vec3(1.0f)), 0.03f);
        dHead = std::min(dHead, sdEllipsoid(ph, vec3(0.0f, 0.1f, 0.3f), vec3(0.15f, 0.08f, 0.15f)));
        
        if (dHead < res.x) res = vec3(dHead, 4.0f, 0.0f);
        
        res.x += 0.0007f * std::sin(150.0f * p.x) * std::sin(150.0f * p.z) * std::sin(150.0f * p.y);
        
        // Legs
        vec3 k1 = vec3(0.42f, -0.05f, 0.92f);
        vec3 k2 = vec3(0.49f, -0.2f, 1.05f);
        float dLegs = 10.0f;
        
        float sx = sign(p.x);
        p.x = std::abs(p.x);
        for (int k = 0; k < 3; k++) {
            vec3 q = p;
            q.y -= std::min(sx, 0.0f) * 0.1f;
            if (k == 0) q += vec3(0.0f, 0.11f, 0.0f);
            if (k == 1) q += vec3(-0.3f, 0.1f, 0.2f);
            if (k == 2) q += vec3(-0.3f, 0.1f, 0.6f);
            
            vec2 se = sdLine(q, vec3(0.3f, 0.1f, 0.8f), k1);
            se.x -= 0.015f + 0.15f * se.y * se.y * (1.0f - se.y);
            dLegs = std::min(dLegs, se.x);
            
            se = sdLine(q, k1, k2);
            se.x -= 0.01f + 0.01f * se.y;
            dLegs = std::min(dLegs, se.x);
            
            se = sdLine(q, k2, k2 + vec3(0.1f, 0.0f, 0.1f));
            se.x -= 0.02f - 0.01f * se.y;
            dLegs = std::min(dLegs, se.x);
        }
        
        if (dLegs < res.x) res = vec3(dLegs, 5.0f, 0.0f);
        
        return res;
    }
    
    inline vec3 worldToMushrom(const vec3& pos) {
        vec3 qos = pos;
        // Rotation matrix equivalent
        qos.x = (60.0f * pos.x + 11.0f * pos.y) / 61.0f;
        qos.y = (-11.0f * pos.x + 60.0f * pos.y) / 61.0f;
        qos.y += 0.03f * std::sin(3.0f * qos.z - 2.0f * std::sin(3.0f * qos.x));
        qos.y -= 0.4f;
        return qos;
    }
    
    inline vec3 mapMushroom(const vec3& pos, const vec3& cur) {
        vec3 res = cur;
        
        vec3 qos = worldToMushrom(pos);
        float db = length(qos - vec3(0.0f, 1.2f, 0.0f)) - 1.3f;
        if (db < cur.x) {
            // Cap
            float d1 = sdEllipsoid(qos, vec3(0.0f, 1.4f, 0.0f), vec3(0.8f, 1.0f, 0.8f));
            float d2 = sdEllipsoid(qos, vec3(0.0f, 0.5f, 0.0f), vec3(1.3f, 1.2f, 1.3f));
            float d = smax(d1, -d2, 0.1f) * 0.8f;
            if (d < res.x) {
                res = vec3(d, 1.0f, 0.0f);
            }
            
            // Stem
            vec3 p = pos;
            p.x += 0.3f * std::sin(p.y) - 0.65f;
            float pa = std::sin(20.0f * std::atan2(p.z, p.x));
            vec2 se = sdLine(p, vec3(0.0f, 2.0f, 0.0f), vec3(0.0f));
            
            float tt = 0.25f - 0.1f * 4.0f * se.y * (1.0f - se.y);
            float d3 = se.x - tt;
            
            d3 = smin(d3, sdEllipsoid(p, vec3(0.0f, 1.7f - 2.0f * (p.x * p.x + p.z * p.z), 0.0f), vec3(0.3f, 0.05f, 0.3f)), 0.05f);
            d3 += 0.003f * pa;
            d3 *= 0.7f;
            
            if (d3 < res.x) {
                res = vec3(d3, 2.0f, 0.0f) * 0.7f;
            }
        }
        return res;
    }
    
    inline vec3 worldToLadyBug(const vec3& p) {
        vec3 q = 4.0f * (p - vec3(-0.0f, 3.2f - 0.6f, -0.57f));
        q = rotateY(rotateZ(rotateX(q, -0.92f), 0.49f), 3.5f);
        q.y += 0.2f;
        return q;
    }
    
    inline vec3 mapShroom(const vec3& pos) {
        vec3 res = vec3(10.0f, 0.0f, 0.0f);
        
        vec3 m1 = pos - vec3(0.0f, 0.1f, 0.0f);
        res = mapMushroom(m1, res);
        
        vec3 q = worldToLadyBug(pos);
        vec3 d3 = mapLadyBug(q, res.x * 4.0f);
        d3.x /= 4.0f;
        if (d3.x < res.x) res = d3;
        
        return res;
    }
}

inline float Mushroom(const vec3& p, float /*time*/, uint32_t /*seed*/) {
    vec3 pTrans = p - vec3(0.2f, -0.7f, 0.0f);
    pTrans = rotationMatrix(vec3(0.0f, 1.0f, 0.0f), pi) * pTrans;
    const float scale = 0.55f;
    return mushroom_detail::mapShroom(pTrans * (1.0f / scale)).x * scale;
}

} // namespace nature
} // namespace sdf

