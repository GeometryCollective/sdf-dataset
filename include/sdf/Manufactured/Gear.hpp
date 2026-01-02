#pragma once
// Ported from Gear.glsl
// Copyright 2013 @P_Malin - CC BY-NC-SA 3.0

#include "../common.hpp"

namespace sdf::manufactured {

namespace detail {
    inline float sdBox(const vec3& p, const vec3& b) {
        vec3 d = abs(p) - b;
        return std::min(std::max(std::max(d.x, d.y), d.z), 0.0f) + length(max(d, vec3(0.0f)));
    }
    
    inline vec3 DomainRotateSymmetry(const vec3& vPos, float fSteps) {
        float angle = std::atan2(vPos.x, vPos.z);
        
        float fScale = fSteps / (pi * 2.0f);
        float steppedAngle = std::floor(angle * fScale + 0.5f) / fScale;
        
        float s = std::sin(-steppedAngle);
        float c = std::cos(-steppedAngle);
        
        return vec3(c * vPos.x + s * vPos.z, vPos.y, -s * vPos.x + c * vPos.z);
    }
    
    inline float GetDistanceGear(const vec3& vPos) {
        float fOuterCylinder = length(vec2(vPos.x, vPos.z)) - 1.05f;
        if (fOuterCylinder > 0.5f) {
            return fOuterCylinder;
        }
        
        vec3 vToothDomain = DomainRotateSymmetry(vPos, 16.0f);
        vToothDomain.x = std::abs(vToothDomain.x);
        vToothDomain.z = std::abs(vToothDomain.z);
        float fGearDist = dot(vec2(vToothDomain.x, vToothDomain.z), normalize(vec2(1.0f, 0.55f))) - 0.55f;
        float fSlabDist = std::abs(vPos.y + 0.1f) - 0.15f;
        
        vec3 vHoleDomain = abs(vPos);
        vHoleDomain -= 0.35f;
        float fHoleDist = length(vec2(vHoleDomain.x, vHoleDomain.z)) - 0.2f;
        
        float fBarDist = vToothDomain.z - 0.15f;
        fBarDist = std::max(vPos.y - 0.1f, fBarDist);
        
        float fResult = fGearDist;
        fResult = std::max(fResult, fSlabDist);
        fResult = std::max(fResult, fOuterCylinder);
        fResult = std::max(fResult, -fHoleDist);
        
        fResult = std::min(fResult, fBarDist);
        return fResult;
    }
}

inline float Gear(const vec3& p, float /*time*/, uint32_t /*seed*/) {
    float boxD = detail::sdBox(p, vec3(1.0f, 1.0f, 1.0f));
    vec3 pRot = rotationMatrix(vec3(1.0f, 0.0f, 0.0f), -pi / 2.0f) * p;
    const float scale = 0.8f;
    return std::max(boxD, detail::GetDistanceGear(pRot * (1.0f / scale))) * scale;
}

} // namespace sdf::manufactured


