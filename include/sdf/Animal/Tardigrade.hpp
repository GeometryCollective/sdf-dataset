#pragma once
// Ported from Tardigrade.glsl
// Copyright 2018 Zguerrero @zguerrero - CC BY-NC-SA 3.0

#include "../common.hpp"

namespace sdf {
namespace animal {

namespace tardigrade_detail {
    constexpr float halfpi = 1.57079632679f;
    
    inline vec4 RotationToQuaternion(const vec3& axis, float angle) {
        float half_angle = angle * halfpi / 180.0f;
        vec2 s = sin(vec2(half_angle, half_angle + halfpi));
        return vec4(axis * s.x, s.y);
    }
    
    inline vec3 Rotate(const vec3& pos, const vec3& axis, float angle) {
        vec3 a = normalize(axis);
        vec4 q = RotationToQuaternion(a, angle);
        vec3 qxyz = vec3(q.x, q.y, q.z);
        return pos + 2.0f * cross(qxyz, cross(qxyz, pos) + q.w * pos);
    }
    
    inline float sdSphere(const vec3& p, float s) { return length(p) - s; }
    
    inline float sdEllipsoid(const vec3& p, const vec3& r) {
        return (length(p / r) - 1.0f) * std::min(std::min(r.x, r.y), r.z);
    }
    
    inline vec3 opRep(const vec3& p, const vec3& c) { return mod(p, c) - 0.5f * c; }
    
    inline float Claws(vec3 pos, const vec3& size, const vec4& angles) {
        vec3 a = pos.y * angles.w + vec3(angles.x, angles.y, angles.z);
        float c1 = sdEllipsoid(Rotate(pos, vec3(0.0f, 0.0f, 1.0f), a.x), size);
        float c2 = sdEllipsoid(Rotate(pos + vec3(0.0f, 0.0f, size.x), vec3(1.0f, 0.0f, 1.0f), a.y), size);
        float c3 = sdEllipsoid(Rotate(pos - vec3(0.0f, 0.0f, size.x), vec3(-1.0f, 0.0f, 1.0f), a.z), size);
        return std::max(std::min(std::min(c1, c2), c3), pos.y);
    }
    
    inline float Leg(vec3 pos, const vec3& axis, float angle, const vec3& size, const vec4& angles) {
        pos = Rotate(pos, axis, angle);
        float claw = Claws(pos + vec3(0.0f, size.y * 0.5f, 0.0f), vec3(0.075f, 0.75f, 0.075f) * size.y, angles);
        float leg = sdEllipsoid(pos, size);
        return std::min(leg, claw);
    }
    
    inline float Teeth(vec3 pos) {
        vec3 polarPos;
        polarPos.x = std::atan2(pos.x, pos.y) / 3.14f;
        polarPos.y = length(vec2(pos.x, pos.y)) - 0.12f;
        polarPos.z = pos.z;
        
        vec3 p = opRep(polarPos, vec3(0.25f, 7.0f, 0.0f));
        p.y = polarPos.y;
        p.z = pos.z;
        
        return sdEllipsoid(p, vec3(0.07f, 0.05f, 0.07f));
    }
    
    inline float Tardigrade(const vec3& pos) {
        float s = 0.01f;
        // Body
        float bodyCenter = sdEllipsoid(Rotate(pos, vec3(1.0f, 0.0f, 0.0f), 10.0f), vec3(1.2f, 0.9f, 1.0f));
        float bodyFront = sdEllipsoid(Rotate(pos + vec3(0.0f, 0.1f, 0.8f), vec3(1.0f, 0.0f, 0.0f), 20.0f), vec3(1.0f, 0.7f, 0.9f));
        float bodyFront2 = sdEllipsoid(Rotate(pos + vec3(0.0f, 0.3f, 1.5f), vec3(1.0f, 0.0f, 0.0f), 40.0f), vec3(0.7f, 0.5f, 0.7f));
        float bodyBack = sdEllipsoid(Rotate(pos + vec3(0.0f, 0.0f, -0.6f), vec3(1.0f, 0.0f, 0.0f), -10.0f), vec3(1.0f, 0.75f, 1.0f));
        float bodyBackHole = sdEllipsoid(pos + vec3(0.0f, 0.2f, -1.5f), vec3(0.03f, 0.03f, 0.5f));
        
        float body = smax(smin(smin(bodyCenter, smin(bodyFront, bodyFront2, s), s), bodyBack, s), -bodyBackHole, 0.15f);
        
        // Mouth
        float mouth0 = sdSphere(pos + vec3(0.0f, 0.7f, 2.25f), 0.15f);
        float mouth1 = sdEllipsoid(pos + vec3(0.0f, 0.6f, 2.125f), vec3(0.22f, 0.175f, 0.175f));
        float teeth0 = Teeth(Rotate(pos + vec3(0.0f, 0.62f, 2.15f), vec3(1.0f, 0.0f, 0.0f), 35.0f));
        
        // Head
        float head = sdEllipsoid(Rotate(pos + vec3(0.0f, 0.45f, 1.9f), vec3(1.0f, 0.0f, 0.0f), 50.0f), vec3(0.45f, 0.3f, 0.5f));
        head = std::min(smax(smin(mouth1, smax(head, -mouth0, 0.3f), s), -mouth0, 0.02f), teeth0);
        
        vec3 symPos = vec3(-std::abs(pos.x), pos.y, pos.z);
        vec3 p;
        
        // Legs
        p = Rotate(symPos + vec3(0.75f, 0.5f, -1.15f), vec3(1.0f, 0.0f, -1.0f), 20.0f);
        float leg0 = Leg(p, vec3(1.0f, 0.0f, 0.0f), 0.0f, vec3(0.2f, 0.5f, 0.25f), vec4(20.0f, -10.0f, -10.0f, 30.0f));
        
        p = Rotate(symPos + vec3(1.0f, 0.55f, 0.0f), vec3(1.0f, 0.0f, -1.0f), 10.0f);
        float leg1 = Leg(p, vec3(1.0f, 0.0f, 0.0f), 0.0f, vec3(0.3f, 0.6f, 0.35f), vec4(25.0f, -5.0f, -10.0f, 40.0f));
        
        p = Rotate(symPos + vec3(0.9f, 0.6f, 1.0f), vec3(1.0f, 0.0f, 1.0f), -5.0f);
        float leg2 = Leg(p, vec3(1.0f, 0.0f, 0.0f), 0.0f, vec3(0.2f, 0.5f, 0.25f), vec4(15.0f, -10.0f, -5.0f, 35.0f));
        
        p = Rotate(symPos + vec3(0.55f, 0.7f, 1.7f), vec3(1.0f, 0.0f, 0.0f), -10.0f);
        float leg3 = Leg(p, vec3(1.0f, 0.0f, 0.0f), 0.0f, vec3(0.15f, 0.3f, 0.15f), vec4(15.0f, -15.0f, -15.0f, 50.0f));
        
        float legs = std::min(std::min(std::min(leg0, leg1), leg2), leg3);
        
        body = smin(body, legs, 0.05f);
        
        return smin(body, head, s);
    }
}

inline float Tardigrade(const vec3& p, float /*time*/, uint32_t /*seed*/) {
    const float scale = 0.3f;
    return tardigrade_detail::Tardigrade(p * (1.0f / scale)) * scale;
}

} // namespace animal
} // namespace sdf

