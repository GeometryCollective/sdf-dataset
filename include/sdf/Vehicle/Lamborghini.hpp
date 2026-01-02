#pragma once
// Ported from Lamborghini.glsl
// Copyright 2019 Florian Berger @flockaroo - CC BY-NC-SA 3.0

#include "../common.hpp"

namespace sdf::vehicle {

namespace lambo_detail {

const vec3 BodySize = vec3(1.8f, 4.14f, 1.0f);

inline float distBox(const vec3& p, const vec3& halfSize) {
    vec3 q = abs(p) - halfSize;
    return length(max(q, vec3(0.0f))) + glm::min(glm::max(q.x, glm::max(q.y, q.z)), 0.0f);
}

inline float distBoxR(const vec3& p, const vec3& halfSize, float r) {
    return distBox(p, halfSize - r) - r;
}

inline float distCyl(const vec3& p, float r, float h) {
    vec2 d = vec2(length(vec2(p.x, p.y)) - r, abs(p.z) - h * 0.5f);
    return glm::min(glm::max(d.x, d.y), 0.0f) + length(max(d, vec2(0.0f)));
}

inline float distCylR(const vec3& p, float r, float h, float R) {
    vec2 d = vec2(length(vec2(p.x, p.y)) - (r - R), abs(p.z) - (h * 0.5f - R));
    return glm::min(glm::max(d.x, d.y), 0.0f) + length(max(d, vec2(0.0f))) - R;
}

inline float distTorus(const vec3& p, float R, float r) {
    return length(p - vec3(normalize(vec2(p.x, p.y)), 0.0f) * R) - r;
}

inline float sminExp(float a, float b, float k) {
    k = 3.0f / k;
    float res = exp2(-k * a) + exp2(-k * b);
    return -log2(res) / k;
}

inline float smin_(float a, float b, float k) {
    float h = clamp(0.5f + 0.5f * (b - a) / k, 0.0f, 1.0f);
    return mix(b, a, h) - k * h * (1.0f - h);
}

inline vec3 rotZ(float ang, const vec3& v) {
    float c = cos(ang), s = sin(ang);
    return vec3(c * v.x - s * v.y, s * v.x + c * v.y, v.z);
}

inline float distTire(const vec3& p_in, float r, float w, float h, float flatness) {
    vec3 p = p_in;
    float l = length(vec2(p.x, p.y));
    float d = 1000.0f;
    float rfl = r * (1.0f + flatness);
    d = glm::min(d, length(vec2(l + rfl - r, p.z)) - rfl);
    d = glm::max(d, length(vec2(l - r + h * 0.5f, p.z)) - w * 0.5f);
    float w_l = sqrt(w * w - h * h);
    float dz = 0.122f * w_l;
    float zfr = mod(p.z, dz);
    float z = p.z - zfr + dz * 0.5f;
    float rz = -(rfl - r) + sqrt(rfl * rfl - p.z * p.z);
    d = glm::max(d, -length(vec2(l - rz, p.z - z)) + dz * 0.2f);
    d = glm::max(d, -(l - (r - h)));
    return d;
}

inline float distRim(const vec3& p_in, float r, float w, float sh) {
    vec3 p0 = p_in;
    vec3 p = p_in;
    p.z = abs(p.z);

    float d = 1000.0f;
    float dmain = distCyl(p, r + sh - 0.005f, w - 0.005f) - 0.005f;
    d = glm::min(d, dmain);

    float d2 = length(p - vec3(0.0f, 0.0f, w * 0.03f + r * 1.5f)) - r * 1.5f;
    d2 = -smin_(-d2, -(distCyl(p - vec3(0.0f, 0.0f, w * 0.5f), r - sh * 1.2f - 0.005f, w * 1.0f - 0.005f) - 0.005f), 0.005f);
    d = -sminExp(-d, d2, 0.01f);

    float ang0 = atan(p.y, p.x);
    float dang = twopi / 15.0f;
    float mang = mod(ang0, dang);
    float ang = ang0 - mang + dang * 0.5f;
    d = glm::max(d, -distBox(rotZ(-ang, p - vec3(r * 0.64f * cos(ang), r * 0.64f * sin(ang), 0.0f)), vec3(0.17f * r, 0.17f * r, w * 1.3f) * 0.52f));

    dang = twopi / 5.0f;
    mang = mod(ang0 + dang * 0.5f, dang);
    ang = ang0 - mang + dang * 0.5f;
    d2 = distCyl(p - vec3(r * 0.28f * cos(ang), r * 0.28f * sin(ang), w * 0.05f), 0.016f, w * 0.19f);
    d = glm::max(d, -d2);
    d = glm::min(d, d2 + 0.005f);
    d = glm::min(d, distCyl(p, 0.03f - 0.01f, w * 0.25f - 0.01f) - 0.01f);

    return d;
}

inline float distWheelDim(const vec3& p, float w_mm, float h_perc, float rimD_inch, float shoulder_mm, float flatness) {
    float w = w_mm * 0.001f;
    float h = w * h_perc / 100.0f;
    float d = 10000.0f, d2;
    float rrim = rimD_inch * 0.5f * 0.0254f;
    d2 = distTire(p, rrim + h, w, h, flatness);
    d = glm::min(d, d2);
    float rimw = sqrt(w * w - h * h) + shoulder_mm * 0.001f;
    d2 = distRim(p, rrim, rimw, shoulder_mm * 0.001f);
    d = glm::min(d, d2);
    return d;
}

inline float distCar(const vec3& p_in) {
    vec3 p;
    p.x = abs(p_in.x);
    p.y = p_in.y;
    p.z = p_in.z;
    vec3 p0 = p;

    float d = 10000.0f, d2;
    p = p0 - vec3(-p.y * 0.01f, 0.0f, 0.0f);
    vec3 p01 = p;
    float yfall = glm::min((p.y + 0.0f) * abs(p.y + 0.0f), 0.0f);
    float yfall2 = (p.y > 0.0f ? 2.5f : 7.0f) * glm::min((-abs(p.y) + 1.2f) * abs(-abs(p.y) + 1.2f), 0.0f);
    yfall = mix(yfall, yfall2, step(0.0f, -p.z));

    // side phase
    d2 = dot(p - 0.51f * BodySize * vec3(0.5f, 0.0f, 1.0f + 0.15f * yfall) - (0.2f + 0.2f * p.y) * glm::max(0.0f, p.x - 0.92f + p.y * 0.03f) * vec3(0.0f, 0.0f, 1.0f),
             normalize(vec3(0.8f, 0.0f, 1.0f - 0.9f * yfall)));
    d = glm::min(d, d2);

    // front cut
    vec3 n = normalize(vec3(0.0f, 1.0f, 2.5f));
    float dpx = glm::max(p.x * 1.0f - 1.0f, -0.5f);
    float dpy = glm::max(p.y - 0.78f + 0.65f * dpx, 0.0f);
    vec3 p2 = p - 0.05f * (1.0f - dpx * 5.0f - dpx * dpx * 10.0f) * (1.0f - exp(-dpy / 0.2f) - dpy * 0.8f);
    d2 = dot(p2 - 0.5f * BodySize * vec3(0.0f, 1.0f, 1.0f) - vec3(0.0f, 0.0f, -0.75f), normalize(vec3(0.0f, 1.0f, 2.5f)));
    d = -smin_(-d, -d2, 0.06f * exp(-(p.y - 1.1f) * (p.y - 1.1f) * 3.0f));

    // main box
    d2 = distBoxR(p + vec3(glm::min(0.35f * p.z * p.z, 0.1f), 0.0f, 0.0f),
                  vec3(BodySize.x, BodySize.z, 100.0f) * 0.5f * vec3(exp(-(step(0.0f, p.y) * 2.0f + 2.0f) * p.y * p.y * p.y * p.y / 500.0f), 1.0f, 1.0f + 0.15f * yfall),
                  glm::max(0.02f, -1.0f * p.z - 0.07f * p.y));
    d = glm::max(d, d2);

    // engine cover
    p -= vec3(0.0f, -1.65f, 0.48f);
    vec3 bs = vec3(BodySize.x * 0.25f * 1.1f - p.y * 0.17f + p.z * 0.4f, 1.0f, 0.3f);
    d2 = distBoxR(p + vec3(0.0f, 0.0f, -p.y * 0.14f), bs, 0.02f);
    d = glm::max(d, -d2);
    float pry = clamp(floor(p.y / 0.22f + 0.5f), 1.0f, 4.0f) * 0.22f;
    d2 = distBoxR(p + vec3(0.0f, -pry, 0.01f - p.y * 0.14f), bs - vec3(0.06f, 0.92f, 0.0f), 0.02f);
    d = glm::max(d, -d2);

    d2 = abs(p0.y) - BodySize.y * 0.5f;
    d2 += 0.005f * exp(-(length(vec2(glm::max(d + 0.03f, 0.0f), d2))) / 0.0025f);
    d = -smin_(-d, -d2, 0.01f);

    d2 = distBox(p01 - vec3(0.0f, -1.1f, 0.3f), vec3(0.47f, 0.5f, 0.5f));
    d += 0.004f * exp(-abs(d2) / 0.004f);

    // side air hole
    p = p0 - vec3(0.9f, -0.35f, 0.04f);
    float sn = (0.6f - 0.4f * sin(p.y * 6.0f));
    d2 = distBox(p, vec3(0.2f * sn, 0.5f, 0.23f * sn) * 0.5f - 0.02f) - 0.02f;
    d = glm::max(d, -d2);

    // upper air hole
    p = p01 - vec3(0.67f, -0.627f, 0.35f);
    d2 = distBoxR(p, vec3(0.32f, 0.45f, 0.25f) * 0.5f, 0.02f - p.y * 0.15f);
    float d3 = dot(p, normalize(vec3(0.58f, -0.5f, 1.0f)));
    float lw = 0.028f + p.x * 0.028f;
    d3 = (fract(d3 / lw) - 0.5f) * lw;
    d3 = abs(d3) - lw * 0.3f;
    d3 = glm::max(d2, -d3);
    d3 = glm::max(d3, (d + 0.01f - p.y * 0.045f));
    d = glm::max(d, -d2);
    d = glm::min(d, d3);

    // door
    d2 = dot(p01 - vec3(0.475f, 0.19f, 0.0f), normalize(vec3(2.0f, -1.0f, 0.0f)));
    d2 = glm::min(d2, dot(p01 - vec3(0.475f, 0.19f, 0.0f), normalize(vec3(1.0f, 0.0f, 0.0f))));
    d3 = d2;
    p = p01 - vec3(0.475f, -0.4f, 0.0f);
    float s = step(0.0f, -(p0.z - 0.2f)) * ((p0.z - 0.2f) * (p0.z - 0.2f) - 0.25f * (p0.z - 0.2f));
    p.y = p.y - s;
    float dr = dot(p, normalize(vec3(0.0f, 1.0f, 0.0f)));
    d2 = glm::min(d2, dr);
    p.y = p.y + 2.0f * s - 1.35f;
    float d4 = 10000.0f;
    d4 = glm::min(d4, dot(p, normalize(vec3(0.0f, -1.0f, 0.0f))));
    d4 = glm::min(d4, dot(p01 - vec3(0.0f, 0.0f, -0.22f), normalize(vec3(0.0f, 0.0f, 1.0f))));
    d2 = glm::min(d2, d4);
    d += 0.003f * exp2(-abs(d2) / 0.003f);

    // side window
    d4 = glm::min(d4, dot(p01 - vec3(0.87f, 0.0f, 0.0f), normalize(vec3(-1.0f, 0.0f, 0.0f))));
    d2 = glm::min(d2, d4);
    d3 -= 0.22f;
    d += 0.005f * exp2(-length(vec2(glm::min(d2 - 0.04f, 0.0f), d)) / 0.0025f);
    if (d3 < glm::min(d4, dr) - 0.04f)
        d = glm::min(d, length(vec2(d3, d)) - 0.003f);

    // front lights
    p = p0 - vec3(0.65f, 1.6f, 0.0f);
    d2 = distBox(p, vec3(0.32f - step(0.0f, p.x) * 0.25f * p.y, 0.16f, 0.2f) * 0.5f);
    d += 0.003f * exp(-abs(d2) / 0.003f);
    p = p0 - vec3(0.63f, 1.85f, 0.0f);
    d2 = distBox(p, vec3(0.29f - step(0.0f, p.x) * 0.45f * p.y, 0.16f, 0.34f) * 0.5f);
    d = glm::max(d, -d2);

    // wheels
    vec3 p1 = BodySize * 0.5f * vec3(1.0f, 0.59f, -0.65f);
    vec3 pw2 = BodySize * 0.5f * vec3(1.0f, -0.63f, -0.65f);
    d = glm::max(d, -distCylR(vec3((p0 - p1).y, (p0 - p1).z, (p0 - p1).x), 0.35f, 0.85f, 0.05f));
    d = glm::max(d, -distBoxR(rotZ(0.4f, vec3((p0 - pw2).y, (p0 - pw2).z, (p0 - pw2).x)), vec3(0.37f, 0.28f, 0.45f) + 0.015f, 0.2f - (p0 - pw2).y * 0.2f));

    // wheel dimension for classic model
    float wheelDimRear[] = {215.0f, 70.0f, 14.0f, 15.0f, 1.0f};
    float wheelDimFront[] = {205.0f, 70.0f, 14.0f, 15.0f, 1.0f};
    vec3 pfront = p1 + vec3(-0.16f, 0.0f, 0.04f);
    vec3 prear = pw2 + vec3(-0.13f, 0.0f, 0.04f);

    bool front = p0.y > 0.0f;
    p = p0 - (front ? pfront : prear);
    d = glm::min(d, distWheelDim(vec3(p.y, p.z, p.x),
                                  front ? wheelDimFront[0] : wheelDimRear[0],
                                  front ? wheelDimFront[1] : wheelDimRear[1],
                                  front ? wheelDimFront[2] : wheelDimRear[2],
                                  front ? wheelDimFront[3] : wheelDimRear[3],
                                  front ? wheelDimFront[4] : wheelDimRear[4]));
    return d;
}

} // namespace lambo_detail

inline float Lamborghini(const vec3& p_in, float /*time*/, uint32_t /*seed*/) {
    vec3 p = p_in;
    const float scale = 0.4f;
    p *= 1.0f / scale;
    mat3 trans = rotationMatrix(vec3(0.0f, 1.0f, 0.0f), -pi) * rotationMatrix(vec3(1.0f, 0.0f, 0.0f), pi / 2.0f);
    return lambo_detail::distCar(trans * p) * scale;
}

} // namespace sdf::vehicle

