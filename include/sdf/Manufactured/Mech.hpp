#pragma once
// Ported from Mech.glsl
// Copyright 2020 @dean_the_coder - CC BY-NC-SA 3.0
// ED-209 from Robocop

#include "../common.hpp"

namespace sdf::manufactured {

namespace mech_detail {

inline mat2 rot(float a) {
    float c = cos(a), s = sin(a);
    return mat2(c, s, -s, c);
}

inline float remap(float f, float in1, float in2, float out1, float out2) {
    return mix(out1, out2, clamp((f - in1) / (in2 - in1), 0.0f, 1.0f));
}

inline float sdBox(const vec3& p, const vec3& b) {
    vec3 q = abs(p) - b;
    return length(max(q, vec3(0.0f))) + glm::min(glm::max(q.x, glm::max(q.y, q.z)), 0.0f);
}

inline float sdChamferedCube(const vec3& p_in, const vec3& r, float c) {
    float cube = sdBox(p_in, r);
    vec3 p = p_in;
    mat2 rotm = rot(pi / 4.0f);
    vec2 pxz = rotm * vec2(p.x, p.z);
    p.x = pxz.x; p.z = pxz.y;
    vec3 rr = r;
    rr.x *= -c / 1.41f + 1.41f;
    rr.z *= -c / 1.41f + 1.41f;
    return glm::max(cube, sdBox(p, rr));
}

inline float sdTriPrism(const vec3& p, const vec2& h) {
    vec3 q = abs(p);
    return glm::max(q.z - h.y, glm::max(q.x * 0.866025f + p.y * 0.5f, -p.y) - h.x * 0.5f);
}

inline float sdCappedCone(const vec3& p, const vec3& a, const vec3& b, float ra, float rb) {
    float rba = rb - ra;
    float baba = dot(b - a, b - a);
    float papa = dot(p - a, p - a);
    float paba = dot(p - a, b - a) / baba;
    float x = sqrt(papa - paba * paba * baba);
    float cax = glm::max(0.0f, x - ((paba < 0.5f) ? ra : rb));
    float cay = abs(paba - 0.5f) - 0.5f;
    float k = rba * rba + baba;
    float f = clamp((rba * (x - ra) + paba * baba) / k, 0.0f, 1.0f);
    float cbx = x - ra - f * rba;
    float cby = paba - f;
    float s = (cbx < 0.0f && cay < 0.0f) ? -1.0f : 1.0f;
    return s * sqrt(glm::min(cax * cax + cay * cay * baba, cbx * cbx + cby * cby * baba));
}

inline float sdCappedCylinder(const vec3& p, float h, float r) {
    vec2 d = abs(vec2(length(vec2(p.x, p.y)), p.z)) - vec2(h, r);
    return glm::min(glm::max(d.x, d.y), 0.0f) + length(max(d, vec2(0.0f)));
}

inline float sdCapsule(const vec3& p, const vec3& a, const vec3& b, float r) {
    vec3 pa = p - a, ba = b - a;
    float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0f, 1.0f);
    return length(pa - ba * h) - r;
}

struct MarchData {
    float d;
    vec3 mat;
    float specPower;
};

inline MarchData minResult(const MarchData& a, const MarchData& b) {
    return (a.d < b.d) ? a : b;
}

inline void setBodyMaterial(MarchData& mat) {
    mat.mat = vec3(0.36f, 0.45f, 0.5f);
    mat.specPower = 30.0f;
}

inline float legWalkAngle(float f, float edWalk) {
    return sin(edWalk * pi * 6.0f * f) * 0.2f;
}

inline float edZ(float edWalk) {
    return mix(5.0f, -2.0f, edWalk);
}

inline float fireShock(float edShoot) {
    return abs(sin(edShoot * pi * 25.0f));
}

inline float headSphere(const vec3& p) {
    return (length(p / vec3(1.0f, 0.8f, 1.0f)) - 1.0f) * 0.8f;
}

inline MarchData headVisor(const vec3& p_in, float h, float bump) {
    vec3 p = p_in;
    bump *= sin(p.x * 150.0f) * sin(p.y * 150.0f) * 0.002f;
    MarchData result;
    result.d = sdBox(p, vec3(1.0f, h, 2.0f));
    result.d = glm::max(mix(result.d, headSphere(p), 0.57f), -p.y) - bump;
    result.mat = vec3(0.05f);
    result.specPower = 30.0f;
    return result;
}

inline MarchData headLower(const vec3& p_in, float gunsUp) {
    vec3 op = p_in;
    vec3 p = p_in;
    MarchData r = headVisor(p * vec3(0.95f, -1.4f, 0.95f), 1.0f, 0.0f);
    
    float roof = glm::max(glm::max(headVisor((p + vec3(0.0f, 0.01f, 0.0f)) * vec3(0.95f), 1.0f, 0.0f).d, p.y - 0.35f), 
                         p.y * 0.625f - p.z - 0.66f);
    r.d = glm::min(r.d, roof);

    // Wings
    mat2 rotm = rot(0.075f * (gunsUp - 1.0f) * sign(p.x));
    vec2 pxy = rotm * vec2(p.x, p.y);
    p.x = pxy.x; p.y = pxy.y;
    p.x = abs(p.x) - 1.33f;
    p.y -= 0.1f - p.x * 0.1f;
    r.d = glm::min(r.d, sdBox(p, vec3(0.4f, 0.06f * (1.0f - p.x), 0.3f - (p.x * 0.2f))));
    p = op;

    // Mouth grill
    p.y = abs(abs(p.y + 0.147f) - 0.1f * 0.556f) - 0.05f * 0.556f;
    r.d = glm::max(r.d, -sdBox(p + vec3(0.0f, 0.0f, 1.5f), vec3(mix(0.25f, 0.55f, -op.y), 0.015f, 0.1f)));

    // Cheeks
    p = op;
    p.y = abs(p.y + 0.16f) - 0.06f;
    p.z -= -1.1f;
    float cheeks = glm::max(sdCappedCylinder(vec3(p.x, p.z, p.y), 1.0f, 0.03f), -sdCappedCylinder(vec3(p.x, p.z, p.y), 0.55f, 1.0f));
    cheeks = glm::max(cheeks, p.z + 0.2f);
    r.d = glm::max(r.d, -cheeks);

    setBodyMaterial(r);
    return r;
}

inline MarchData gunPod(const vec3& p_in, float edShoot, float& glow) {
    MarchData r;
    setBodyMaterial(r);

    vec3 p = p_in;
    p.y += 0.1f;
    p.z += 0.45f;

    const float rr = 0.35f, chamfer = 0.1f;
    vec3 pp = p;
    pp.z = abs(pp.z) - 0.5f;
    r.d = sdCappedCone(pp, vec3(0.0f), vec3(0.0f, 0.0f, -chamfer), rr - chamfer, rr);
    r.d = glm::min(r.d, sdCappedCylinder(p, rr, 0.4f));

    pp = vec3(p.x, 0.28f - p.y, p.z);
    r.d = glm::min(r.d, sdTriPrism(pp, vec2(0.1f, 0.5f)));

    pp = p;
    pp.x = abs(p.x);
    mat2 rotm = rot(pi / 4.0f);
    vec2 ppxy = rotm * vec2(pp.x, pp.y);
    pp.x = ppxy.x; pp.y = ppxy.y;
    float bump = sign(sin(pp.z * 33.3f)) * 0.003f;
    float d = sdBox(pp, vec3(0.1f - bump, 0.38f - bump, 0.34f)) - 0.02f;

    pp = p - vec3(0.0f, 0.0f, -0.6f);
    pp.x = abs(pp.x) - 0.1f;
    d = glm::min(d, sdCappedCylinder(pp, 0.06f, 0.15f));
    d = glm::min(d, sdCappedCylinder(pp + vec3(0.0f, 0.12f, -0.05f), 0.06f, 0.05f));
    d = glm::min(d, sdBox(p + vec3(0.0f, 0.0f, 0.54f), vec3(0.1f, 0.06f, 0.04f)));
    if (d < r.d) {
        d = glm::max(d, -sdCappedCylinder(pp + vec3(0.0f, 0.0f, 0.1f), 0.03f, 0.2f));
        r.d = d;
        r.mat = vec3(0.02f);
    }

    float fs = fireShock(edShoot);
    if (fs > 0.5f) {
        d = sdCappedCylinder(pp, 0.01f + pp.z * 0.05f, fract(fs * 3322.423f) * 0.5f + 0.9f);
        if (d < r.d) {
            r.d = d;
            r.mat = vec3(1.0f);
            glow += 0.1f / (0.01f + d * d * 400.0f);
        }
    }

    return r;
}

inline MarchData arms(const vec3& p_in, float gunsUp, float gunsForward, float edShoot, float& glow) {
    MarchData r;
    setBodyMaterial(r);

    vec3 p = p_in;
    p.x = abs(p.x);
    p.y += 0.24f;
    p.z -= 0.0f;
    mat2 rotm = rot(0.15f * (gunsUp - 1.0f));
    vec2 pxy = rotm * vec2(p.x, p.y);
    p.x = pxy.x; p.y = pxy.y;

    const vec3 elbow = vec3(1.5f, 0.0f, 0.0f);
    const vec3 wrist = elbow - vec3(0.0f, 0.0f, 0.3f);
    r.d = glm::min(sdCapsule(p, vec3(0.0f), elbow, 0.2f), sdCapsule(p, elbow, wrist, 0.2f));

    p -= wrist;
    p.z -= gunsForward * 0.15f;
    return minResult(r, gunPod(p, edShoot, glow));
}

inline float toe(const vec3& p_in) {
    vec3 p = p_in;
    p.y += 0.1f;
    p.z += 0.32f;
    return glm::max(sdBox(p, vec3(0.3f + 0.2f * (p.z - 0.18f) - (p.y * 0.456f) * 0.5f, 
                          0.3f + 0.2f * cos((p.z - 0.18f) * 3.69f), 0.35f)), 0.1f - p.y);
}

inline float foot(const vec3& p_in) {
    vec3 p = p_in;
    p.z += 0.8f;
    mat2 rotm = rot(0.86f);
    vec2 pyz = rotm * vec2(p.y, p.z);
    p.y = pyz.x; p.z = pyz.y;

    float d = toe(p);
    mat2 rotm2 = rot(1.57f);
    vec2 pxz = rotm2 * vec2(p.x, p.z);
    p.x = pxz.x; p.z = pxz.y;
    p.x -= 0.43f;
    p.z = 0.25f - abs(p.z);

    return glm::min(d, toe(p));
}

inline MarchData waist(const vec3& p_in, float edWalk) {
    MarchData r;
    setBodyMaterial(r);

    vec3 p = p_in;
    p.y += 0.65f;
    mat2 rotm = rot(-0.2f);
    vec2 pyz = rotm * vec2(p.y, p.z);
    p.y = pyz.x; p.z = pyz.y;
    float legAngle = legWalkAngle(1.0f, edWalk);
    mat2 rotm2 = rot(legAngle * 0.3f);
    vec2 pxy = rotm2 * vec2(p.x, p.y);
    p.x = pxy.x; p.y = pxy.y;

    vec3 pp = p;
    pp.y += 0.3f;
    r.d = glm::max(sdCappedCylinder(vec3(pp.z, pp.y, pp.x), 0.5f, 0.5f), p.y + 0.15f);

    float bump = 0.5f - abs(sin(p.y * 40.0f)) * 0.03f;
    float d = sdBox(p, vec3(bump, 0.15f, bump));

    bump = 0.3f - abs(sin(p.x * 40.0f)) * 0.03f;
    pp.y += 0.18f;
    d = glm::min(d, sdCappedCylinder(vec3(pp.z, pp.y, pp.x), bump, 0.75f));

    pp.x = abs(pp.x);
    mat2 rotm3 = rot(0.2f - pi / 4.0f + legAngle * sign(p.x));
    vec2 ppyz = rotm3 * vec2(pp.y, pp.z);
    pp.y = ppyz.x; pp.z = ppyz.y;
    pp.x -= 0.98f;
    r.d = glm::min(r.d, glm::max(sdCappedCylinder(vec3(pp.z, pp.y, pp.x), 0.4f, 0.24f), -pp.y));
    r.d = glm::min(r.d, sdBox(pp, vec3(0.24f, 0.2f, 0.14f + 0.2f * pp.y)));

    vec3 cp = pp;
    p = pp;
    pp.x = abs(pp.x) - 0.12f;
    pp.z = abs(pp.z) - 0.25f;
    float pistons = glm::min(sdCappedCylinder(vec3(pp.x, pp.z, pp.y), 0.1f, 0.325f), 
                            sdCappedCylinder(vec3(pp.x, pp.z, pp.y), 0.05f, 0.5f));
    r.d = glm::min(r.d, glm::max(pistons, pp.y));

    p.y += 0.68f;
    r.d = glm::min(r.d, sdBox(p, vec3(sign(abs(p.y) - 0.04f) * 0.005f + 0.26f, 0.2f, 0.34f)));

    if (d < r.d) {
        r.d = d;
        r.mat = vec3(0.02f);
    }

    return r;
}

inline MarchData legs(const vec3& p_in, float edWalk) {
    MarchData r;
    setBodyMaterial(r);

    float legAngle = legWalkAngle(1.0f, edWalk);
    vec3 p = p_in;
    p.z += 0.27f;
    mat2 rotm = rot(legAngle * sign(p.x));
    vec2 pyz = rotm * vec2(p.y, p.z);
    p.y = pyz.x; p.z = pyz.y;
    p.z -= 0.27f;

    p.y += 0.65f;
    mat2 rotm2 = rot(-0.2f);
    vec2 pyz2 = rotm2 * vec2(p.y, p.z);
    p.y = pyz2.x; p.z = pyz2.y;
    mat2 rotm3 = rot(legAngle * 0.3f);
    vec2 pxy = rotm3 * vec2(p.x, p.y);
    p.x = pxy.x; p.y = pxy.y;

    vec3 pp = p;
    pp.x = abs(pp.x);
    pp.y += 0.48f;
    mat2 rotm4 = rot(0.2f - pi / 4.0f);
    vec2 ppyz = rotm4 * vec2(pp.y, pp.z);
    pp.y = ppyz.x; pp.z = ppyz.y;
    pp.x -= 0.98f;

    vec3 cp = pp;
    p = pp;
    pp.x = abs(pp.x) - 0.12f;
    pp.z = abs(pp.z) - 0.25f;
    p.y += 0.68f;

    p.x = abs(p.x) - 0.12f;
    p.y = abs(p.y) - 0.12f;
    float silver = sdBox(p, vec3(0.07f, 0.05f, 1.2f));

    cp -= vec3(0.0f, -0.7f, 0.0f);
    r.d = sdBox(cp - vec3(0.0f, 0.0f, 1.15f), vec3(0.17f, 0.17f, 0.07f)) - 0.04f;

    cp.z += 1.0f;
    r.d = glm::min(r.d, sdChamferedCube(vec3(cp.x, cp.z, cp.y), vec3(0.28f - sign(abs(cp.z) - 0.3f) * 0.01f, 0.5f, 0.28f - sign(abs(cp.z) - 0.3f) * 0.01f), 0.18f));
    r.d = glm::min(r.d, foot(cp));

    if (silver < r.d) {
        r.d = silver;
        r.mat = vec3(0.8f);
    }

    return r;
}

inline MarchData ed209(const vec3& p_in, float stretch, float edWalk, float edDown, float edTwist, float edShoot) {
    float glow = 0.0f;
    vec3 p = p_in;
    p.y += legWalkAngle(2.0f, edWalk) * 0.2f + 0.1f;
    p.z -= edZ(edWalk);

    MarchData r = legs(p, edWalk);

    float f = glm::min(stretch * 2.0f, 1.0f);
    float slide = f < 0.5f ? smoothstep(0.0f, 0.5f, f) : (1.0f - smoothstep(0.5f, 1.0f, f) * 0.2f);
    p.y -= slide * 0.5f;
    p.z -= slide * 0.5f;
    float gunsUp = smoothstep(0.0f, 1.0f, clamp((stretch - 0.66f) * 6.0f, 0.0f, 1.0f));
    float gunsForward = smoothstep(0.0f, 1.0f, clamp((stretch - 0.83f) * 6.0f, 0.0f, 1.0f))
                        + fireShock(edShoot) * 0.5f;
    r = minResult(r, waist(p, edWalk));

    mat2 rotm = rot(0.1f * (-edDown + legWalkAngle(2.0f, edWalk) + smoothstep(0.0f, 1.0f, clamp((stretch - 0.5f) * 6.0f, 0.0f, 1.0f)) - 1.0f));
    vec2 pyz = rotm * vec2(p.y, p.z);
    p.y = pyz.x; p.z = pyz.y;
    mat2 rotm2 = rot(edTwist * 0.2f);
    vec2 pxz = rotm2 * vec2(p.x, p.z);
    p.x = pxz.x; p.z = pxz.y;
    r = minResult(r, headLower(p, gunsUp));
    r = minResult(r, headVisor(p, 0.8f, 1.0f));

    return minResult(r, arms(p, gunsUp, gunsForward, edShoot, glow));
}

} // namespace mech_detail

inline float Mech(const vec3& p_in, float time, uint32_t /*seed*/) {
    vec3 p = p_in;
    p = p * rotationMatrix(vec3(0.0f, 1.0f, 0.0f), pi);
    p += vec3(-0.11f, -0.25f, 2.0f);
    const float scale = 0.4f;
    p *= 1.0f / scale;
    float stretch = 2.0f * (sin(time * 2.0f) + 1.0f);
    return mech_detail::ed209(p, stretch, 0.0f, 0.0f, 0.0f, 0.0f).d * scale * 0.8f;
}

} // namespace sdf::manufactured

