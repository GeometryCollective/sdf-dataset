#pragma once

// Common utilities for translating GLSL SDFs to C++
// This header provides GLSL-like functions and types using glm

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/norm.hpp>

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace sdf {

// ============================================================================
// Type aliases matching GLSL naming
// ============================================================================

using vec2 = glm::vec2;
using vec3 = glm::vec3;
using vec4 = glm::vec4;
using mat2 = glm::mat2;
using mat3 = glm::mat3;
using mat4 = glm::mat4;
using ivec2 = glm::ivec2;
using ivec3 = glm::ivec3;

// ============================================================================
// Constants
// ============================================================================

constexpr float pi = 3.14159265358979323846f;
constexpr float twopi = 2.0f * pi;

// ============================================================================
// GLSL built-in functions (those not directly available in glm)
// ============================================================================

// fract - return fractional part
inline float fract(float x) {
    return x - std::floor(x);
}

inline vec2 fract(const vec2& v) {
    return vec2(fract(v.x), fract(v.y));
}

inline vec3 fract(const vec3& v) {
    return vec3(fract(v.x), fract(v.y), fract(v.z));
}

// mod - modulo (GLSL-style, handles negatives differently than C++ %)
inline float mod(float x, float y) {
    return x - y * std::floor(x / y);
}

inline vec2 mod(const vec2& v, float y) {
    return vec2(mod(v.x, y), mod(v.y, y));
}

inline vec3 mod(const vec3& v, float y) {
    return vec3(mod(v.x, y), mod(v.y, y), mod(v.z, y));
}

inline vec2 mod(const vec2& v, const vec2& y) {
    return vec2(mod(v.x, y.x), mod(v.y, y.y));
}

inline vec3 mod(const vec3& v, const vec3& y) {
    return vec3(mod(v.x, y.x), mod(v.y, y.y), mod(v.z, y.z));
}

// mix - linear interpolation (glm has this but we provide convenient overloads)
using glm::mix;

// clamp
using glm::clamp;

// smoothstep
using glm::smoothstep;

// step
inline float step(float edge, float x) {
    return x < edge ? 0.0f : 1.0f;
}

inline vec2 step(const vec2& edge, const vec2& x) {
    return vec2(step(edge.x, x.x), step(edge.y, x.y));
}

inline vec3 step(const vec3& edge, const vec3& x) {
    return vec3(step(edge.x, x.x), step(edge.y, x.y), step(edge.z, x.z));
}

inline vec2 step(float edge, const vec2& x) {
    return vec2(step(edge, x.x), step(edge, x.y));
}

inline vec3 step(float edge, const vec3& x) {
    return vec3(step(edge, x.x), step(edge, x.y), step(edge, x.z));
}

// sign
using std::copysign;

inline float sign(float x) {
    if (x > 0.0f) return 1.0f;
    if (x < 0.0f) return -1.0f;
    return 0.0f;
}

inline vec2 sign(const vec2& v) {
    return vec2(sign(v.x), sign(v.y));
}

inline vec3 sign(const vec3& v) {
    return vec3(sign(v.x), sign(v.y), sign(v.z));
}

// abs
using std::abs;

inline vec2 abs(const vec2& v) {
    return glm::abs(v);
}

inline vec3 abs(const vec3& v) {
    return glm::abs(v);
}

inline vec4 abs(const vec4& v) {
    return glm::abs(v);
}

// min/max with vectors
using glm::min;
using glm::max;

inline float min3(float a, float b, float c) {
    return std::min(a, std::min(b, c));
}

inline float max3(float a, float b, float c) {
    return std::max(a, std::max(b, c));
}

// length, normalize, dot, cross
using glm::length;
using glm::normalize;
using glm::dot;
using glm::cross;
using glm::reflect;
using glm::distance;

// floor, ceil, round
using std::floor;
using std::ceil;
using std::round;

inline vec2 floor(const vec2& v) {
    return glm::floor(v);
}

inline vec3 floor(const vec3& v) {
    return glm::floor(v);
}

inline vec2 ceil(const vec2& v) {
    return glm::ceil(v);
}

inline vec3 ceil(const vec3& v) {
    return glm::ceil(v);
}

// sqrt, pow, exp, log
using std::sqrt;
using std::pow;
using std::exp;
using std::log;

inline vec2 sqrt(const vec2& v) {
    return glm::sqrt(v);
}

inline vec3 sqrt(const vec3& v) {
    return glm::sqrt(v);
}

inline vec3 pow(const vec3& v, const vec3& e) {
    return glm::pow(v, e);
}

// Trigonometry
using std::sin;
using std::cos;
using std::tan;
using std::asin;
using std::acos;
using std::atan;
using std::atan2;
using std::sinh;
using std::cosh;
using std::tanh;

inline vec2 sin(const vec2& v) {
    return glm::sin(v);
}

inline vec3 sin(const vec3& v) {
    return glm::sin(v);
}

inline vec2 cos(const vec2& v) {
    return glm::cos(v);
}

inline vec3 cos(const vec3& v) {
    return glm::cos(v);
}

// atan with two arguments (GLSL-style)
inline float atan(float y, float x) {
    return std::atan2(y, x);
}

// ============================================================================
// Common SDF helper functions
// ============================================================================

// Smooth minimum (polynomial)
inline float smin(float a, float b, float k) {
    float h = clamp(0.5f + 0.5f * (b - a) / k, 0.0f, 1.0f);
    return mix(b, a, h) - k * h * (1.0f - h);
}

// Smooth maximum
inline float smax(float a, float b, float k) {
    return -smin(-a, -b, k);
}

// Rotation matrix around arbitrary axis
inline mat3 rotationMatrix(const vec3& axis, float angle) {
    vec3 a = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0f - c;
    
    return mat3(
        oc * a.x * a.x + c,        oc * a.x * a.y - a.z * s,  oc * a.z * a.x + a.y * s,
        oc * a.x * a.y + a.z * s,  oc * a.y * a.y + c,        oc * a.y * a.z - a.x * s,
        oc * a.z * a.x - a.y * s,  oc * a.y * a.z + a.x * s,  oc * a.z * a.z + c
    );
}

// 2D rotation matrix
inline mat2 rot2D(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return mat2(c, s, -s, c);
}

// Simple hash functions for procedural generation
inline float hash11(float p) {
    p = fract(p * 0.1031f);
    p *= p + 33.33f;
    p *= p + p;
    return fract(p);
}

inline float hash12(const vec2& p) {
    vec3 p3 = fract(vec3(p.x, p.y, p.x) * 0.1031f);
    p3 += dot(p3, vec3(p3.y, p3.z, p3.x) + 33.33f);
    return fract((p3.x + p3.y) * p3.z);
}

inline float hash13(const vec3& p3) {
    vec3 p = fract(p3 * 0.1031f);
    p += dot(p, vec3(p.y, p.z, p.x) + 33.33f);
    return fract((p.x + p.y) * p.z);
}

inline vec3 hash33(const vec3& p3) {
    vec3 p = fract(p3 * vec3(0.1031f, 0.1030f, 0.0973f));
    p += dot(p, vec3(p.y, p.x, p.z) + 33.33f);
    return fract((vec3(p.x, p.x, p.y) + vec3(p.y, p.z, p.z)) * vec3(p.z, p.y, p.x));
}

// Seeded random number generator (simple LCG-based)
inline float seededRandom(uint32_t& seed) {
    seed = seed * 1664525u + 1013904223u;
    return static_cast<float>(seed) / 4294967296.0f;
}

} // namespace sdf


