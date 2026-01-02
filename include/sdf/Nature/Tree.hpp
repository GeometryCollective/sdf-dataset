#pragma once
// Ported from Tree.glsl
// Copyright 2020 @Maurogik - CC BY-NC-SA 3.0
// Maple tree with leaves and branches

#include "../common.hpp"

namespace sdf::nature {

namespace tree_detail {

inline float tree_smin(float a, float b, float k) {
    float h = clamp(0.5f + 0.5f * (b - a) / k, 0.0f, 1.0f);
    return mix(b, a, h) - k * h * (1.0f - h);
}

inline float tree_smax(float a, float b, float k) {
    return tree_smin(a, b, -k);
}

inline float fSphere(const vec3& p, float r) {
    return length(p) - r;
}

inline float fCapsule(const vec3& p, float r, float c) {
    return mix(length(vec2(p.x, p.z)) - r, length(vec3(p.x, abs(p.y) - c, p.z)) - r, step(c, abs(p.y)));
}

inline float fLineSegment(const vec3& p, const vec3& a, const vec3& b) {
    vec3 ab = b - a;
    float t = saturate(dot(p - a, ab) / dot(ab, ab));
    return length((ab * t + a) - p);
}

inline float fCapsuleLine(const vec3& p, const vec3& a, const vec3& b, float r) {
    return fLineSegment(p, a, b) - r;
}

inline void pR(float& px, float& py, float a) {
    float c = cos(a), s = sin(a);
    float nx = c * px + s * py;
    float ny = c * py - s * px;
    px = nx; py = ny;
}

inline float pModPolar(float& px, float& py, float repetitions) {
    float angle = twopi / repetitions;
    float a = atan(py, px) + angle / 2.0f;
    float r = length(vec2(px, py));
    float c = floor(a / angle);
    a = mod(a, angle) - angle / 2.0f;
    px = cos(a) * r;
    py = sin(a) * r;
    if (abs(c) >= (repetitions / 2.0f)) c = abs(c);
    return c;
}

inline vec3 pMod3(vec3& p, const vec3& size) {
    vec3 c = floor((p + size * 0.5f) / size);
    p = mod(p + size * 0.5f, size) - size * 0.5f;
    return c;
}

inline vec3 opCheapBend(const vec3& p, float bend) {
    float k = bend;
    float c = cos(k * p.x);
    float s = sin(k * p.x);
    mat2 m = mat2(c, -s, s, c);
    vec2 bent = m * vec2(p.x, p.y);
    return vec3(bent.x, bent.y, p.z);
}

inline float sdTriangleIsosceles(const vec2& p_in, const vec2& q) {
    vec2 p = vec2(abs(p_in.x), p_in.y);
    vec2 a = p - q * clamp(dot(p, q) / dot(q, q), 0.0f, 1.0f);
    vec2 b = p - q * vec2(clamp(p.x / q.x, 0.0f, 1.0f), 1.0f);
    float s = -sign(q.y);
    vec2 d = min(vec2(dot(a, a), s * (p.x * q.y - p.y * q.x)),
                 vec2(dot(b, b), s * (p.y - q.y)));
    return -sqrt(d.x) * sign(d.y);
}

inline float opExtrusion(const vec3& p, float dist, float h) {
    vec2 w = vec2(dist, abs(p.z) - h);
    return glm::min(glm::max(w.x, w.y), 0.0f) + length(max(w, vec2(0.0f)));
}

// Simplified maple leaf
inline float fMapleLeaf(const vec3& posLeaf_in, float scale, float rand) {
    vec3 posLeaf = opCheapBend(vec3(posLeaf_in.x, posLeaf_in.z, posLeaf_in.y), (rand - 0.5f) * 10.0f);
    posLeaf = vec3(posLeaf.x, posLeaf.z, posLeaf.y);
    
    vec3 posTri = posLeaf - vec3(0.0f, 1.0f, 0.0f) * scale;
    posTri.y = -posTri.y;
    
    float dist2D = sdTriangleIsosceles(vec2(posTri.x, posTri.y), vec2(scale * 1.2f, scale * 2.0f));
    
    posTri.y -= scale * 0.75f;
    posTri.x = abs(posTri.x) - scale * 1.3f;
    pR(posTri.x, posTri.y, pi * 0.35f);
    
    float distTriSides = sdTriangleIsosceles(vec2(posTri.x, posTri.y), vec2(scale * 0.55f, scale * 1.3f));
    dist2D = glm::min(dist2D, distTriSides);
    
    float minDist = opExtrusion(posLeaf, dist2D, 0.005f);
    
    // Stick
    vec3 posStick = posLeaf + vec3(0.0f, 1.0f, 0.0f) * scale * 0.5f;
    float stickDist = fCapsuleLine(posStick, -vec3(0.0f, 1.0f, 0.0f) * scale * 1.0f, vec3(0.0f), 0.003f);
    minDist = glm::min(minDist, stickDist);
    
    return minDist;
}

inline float fBranchSDF(const vec3& posBranch_in, float len, float rad, float rand) {
    vec3 posBranch = posBranch_in;
    float branchHalfLen = len * 0.5f;
    float progressAlong = posBranch.y / (2.0f * branchHalfLen);
    float branchRad = rad * (1.0f - progressAlong * 0.8f);
    
    float wave = sin((rand + posBranch.y) / len * 12.0f) * 0.25f * rad;
    posBranch.x += wave;
    posBranch.z += wave;
    
    return fCapsule(posBranch - vec3(0.0f, branchHalfLen, 0.0f), branchRad, branchHalfLen);
}

inline float fBranchSDF(const vec3& posWS, float scale, float rand) {
    float branchLen = 1.0f * scale;
    float branchRad = 0.03f * scale;
    return fBranchSDF(posWS, branchLen, branchRad, rand);
}

inline float fSmallBranchesSDF(const vec3& posWS_in, float branchDist, float branchProgress) {
    vec3 posWS = posWS_in + vec3(0.53f, 1.0f, 0.53f);
    
    vec3 posBranches = posWS;
    vec3 id = pMod3(posBranches, vec3(2.0f));
    pModPolar(posBranches.y, posBranches.z, 3.0f);
    float rand = hash13(id * 789.5336f);
    posBranches.x += sin((posBranches.y + rand) * twopi) * 0.05f;
    posBranches.z += sin((posBranches.y + rand) * twopi) * 0.05f;
    pR(posBranches.x, posBranches.y, (rand - 0.5f) * pi * 0.25f);
    float branchLen = clamp(branchDist * 2.0f, 0.5f, 2.0f);
    float rad = 0.05f * (1.3f - ((posBranches.y / branchLen) + 0.5f));
    float minDist = fCapsule(posBranches, rad, branchLen);
    
    // Remove branches near center and past main branches
    minDist += saturate(0.7f - branchProgress);
    minDist = tree_smax(minDist, branchDist - 3.0f, 0.75f);
    minDist += saturate((branchProgress - 1.4f) * 3.0f);
    
    return minDist;
}

inline float fCanopy(const vec3& posTreeSpace, float branchesDist, float branchProgress) {
    const float leafSize = 0.15f;
    const float leafRep = 0.4f;
    
    vec3 leavesPos = posTreeSpace;
    vec3 leafId = pMod3(leavesPos, vec3(leafRep));
    float leafRand = hash13(leafId * 347.0468f);
    leavesPos += (leafRand - 0.5f) * vec3(leafRep * 0.5f);
    pR(leavesPos.x, leavesPos.z, leafRand * pi);
    float leavesDist = fMapleLeaf(leavesPos, leafSize, leafRand);
    
    leavesPos = posTreeSpace + vec3(leafRep * 0.5f);
    pR(leavesPos.x, leavesPos.z, 1.618f * pi);
    leafId = pMod3(leavesPos, vec3(leafRep));
    leafRand = hash13(leafId * 347.0468f);
    pR(leavesPos.x, leavesPos.z, leafRand * pi);
    leavesDist = glm::min(leavesDist, fMapleLeaf(leavesPos, leafSize, leafRand));
    
    // Remove leaves too far from branches
    float branchStart = linearstep(0.6f, 0.4f, branchProgress);
    float branchEnd = linearstep(1.3f, 1.42f, branchProgress);
    leavesDist = glm::max(leavesDist, branchesDist - (0.27f - branchEnd * 0.17f) + branchStart);
    
    return leavesDist;
}

inline float fTreeSDF(const vec3& posTreeSpace, float groundY, float time) {
    float minDist = 10000.0f;
    float treeBoundingSphereDist = fSphere(posTreeSpace - vec3(0.0f, 8.0f, 0.0f), 9.0f);
    
    if (treeBoundingSphereDist > 12.0f) {
        return treeBoundingSphereDist - 10.0f;
    }
    
    if (treeBoundingSphereDist > 1.0f) {
        return treeBoundingSphereDist;
    }
    
    vec3 trunkPos = posTreeSpace;
    
    // Trunk
    float trunkDist = fBranchSDF(trunkPos, 10.0f, 0.5f, 0.0f);
    minDist = glm::min(minDist, trunkDist);
    
    float minBranchDist = 10000.0f;
    float minBranchProgress = 0.0f;
    
    // Wind effect
    float winFlexOffset = (sin(posTreeSpace.x * 0.1f) + sin(posTreeSpace.y * 0.1f) + sin(posTreeSpace.z * 0.1f)) * twopi;
    float windFlexAmount = glm::min(8.0f, trunkDist) / 8.0f;
    vec3 windOffset = vec3(-0.75f, 0.5f, 0.0f) * sin(time * 4.0f + winFlexOffset) * 0.05f * windFlexAmount;
    
    // Branch set 1
    vec3 branchPos = trunkPos;
    float id = pModPolar(branchPos.x, branchPos.z, 6.0f);
    float rand = hash11(id * 736.884f);
    branchPos.y -= 4.0f + 1.0f * rand;
    pR(branchPos.x, branchPos.y, -pi * (0.32f + rand * 0.1f));
    float branchDist = fBranchSDF(branchPos, 5.75f, rand);
    if (branchDist < minBranchDist) {
        minBranchDist = branchDist;
        minBranchProgress = branchPos.y / 5.75f;
    }
    
    // Branch set 2
    branchPos = trunkPos;
    pR(branchPos.x, branchPos.z, -pi * 0.35f);
    id = pModPolar(branchPos.x, branchPos.z, 5.0f);
    rand = hash11(id * 736.884f);
    branchPos.y -= 7.5f + 1.0f * rand;
    pR(branchPos.x, branchPos.y, -pi * (0.35f - rand * 0.05f));
    branchDist = fBranchSDF(branchPos, 5.0f, 0.0f);
    if (branchDist < minBranchDist) {
        minBranchDist = branchDist;
        minBranchProgress = branchPos.y / 5.0f;
    }
    
    // Branch set 3
    branchPos = trunkPos;
    pR(branchPos.x, branchPos.z, -pi * 0.65f);
    id = pModPolar(branchPos.x, branchPos.z, 3.0f);
    rand = hash11(id * 736.884f);
    branchPos.y -= 9.5f + 0.5f * rand;
    pR(branchPos.x, branchPos.y, -pi * (0.22f - 0.1f * rand));
    branchDist = fBranchSDF(branchPos, 4.0f, 0.0f);
    if (branchDist < minBranchDist) {
        minBranchDist = branchDist;
        minBranchProgress = branchPos.y / 4.0f;
    }
    
    minDist = glm::min(minDist, minBranchDist);
    
    // Small branches
    float smallBranchesDist = fSmallBranchesSDF(trunkPos + windOffset * 0.25f, minBranchDist, minBranchProgress);
    minDist = glm::min(minDist, smallBranchesDist);
    
    // Leaves (canopy)
    float leavesDist = fCanopy(trunkPos + windOffset, smallBranchesDist, minBranchProgress);
    minDist = glm::min(minDist, leavesDist);
    
    return minDist;
}

inline float fSDF(const vec3& posWS, float time) {
    // Simplified: just the tree, no mountains or grass
    const vec3 kTreePosWS = vec3(0.0f, 0.0f, -8.0f);
    float groundY = fSphere(posWS - vec3(-10.0f, -500.0f, -20.0f), 500.0f);
    return fTreeSDF(posWS - kTreePosWS, groundY, time);
}

} // namespace tree_detail

inline float Tree(const vec3& p_in, float time, uint32_t /*seed*/) {
    vec3 p = p_in;
    const float scale = 0.1f;
    p *= 1.0f / scale;
    p -= vec3(0.0f, -6.0f, 9.0f);
    return tree_detail::fSDF(p, time) * scale;
}

} // namespace sdf::nature

