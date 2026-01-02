#pragma once
// Ported from GrandPiano.glsl
// Copyright 2014 @jedi_cy - CC BY-NC-SA 3.0

#include "../common.hpp"

namespace sdf::manufactured {

namespace grandpiano_detail {

const float hinge_angle = pi / 2.0f;
const vec3 place_pos = vec3(10.0f, 0.0f, 40.0f);
const float EPSILON = 0.2f;

inline float Combine(float re1, float re2) {
    if (re1 < 0.0f || re2 < 0.0f)
        return glm::max(re1, re2);
    return sqrt(re1 * re1 + re2 * re2);
}

inline float Subtract(float re1, float sub) {
    if (sub > 0.0f) return re1;
    return glm::max(-sub, re1);
}

inline float MapBox(const vec3& pos, const vec3& half_size) {
    vec3 v = abs(pos) - half_size;
    if (v.x < 0.0f || v.y < 0.0f || v.z < 0.0f)
        return glm::max(glm::max(v.x, v.y), v.z);
    return length(v);
}

inline float MapBoxSim(const vec3& pos, const vec3& half_size) {
    return length(max(abs(pos) - half_size, vec3(0.0f))) - 0.1f;
}

inline float MapRoundBox(const vec3& pos, const vec3& half_size, float r) {
    return length(max(abs(pos) - half_size, vec3(0.0f))) - r;
}

inline float Map2Box(const vec3& pos, const vec2& top_half_size, const vec2& bottom_half_size, float half_h) {
    float y = abs(pos.y) - half_h;
    float p = pos.y * 0.5f / half_h + 0.5f;
    p = clamp(p, 0.0f, 1.0f);
    float x = abs(pos.x) - mix(bottom_half_size.x, top_half_size.x, p);
    float z = abs(pos.z) - mix(bottom_half_size.y, top_half_size.y, p);
    if (x < 0.0f || y < 0.0f || z < 0.0f)
        return glm::max(glm::max(x, y), z);
    return sqrt(x * x + y * y + z * z);
}

inline float MapCylinder(const vec3& pos, float r, float half_h) {
    float y = abs(pos.y) - half_h;
    float rr = length(vec2(pos.x, pos.z)) - r;
    return Combine(y, rr);
}

inline float MapPianoBodyShapeDist(float x, float y) {
    if (y > 118.0f) {
        return sqrt((x + 30.0f) * (x + 30.0f) + (y - 118.0f) * (y - 118.0f)) - 45.0f;
    }
    if (y > 42.0f) {
        float vx = -x - 75.0f;
        float sinv = sin(((y - 42.0f) / 76.0f + 0.5f) * pi);
        sinv = x - (sinv * 30.0f + 45.0f);
        if (x < -30.0f) return vx;
        return glm::max(sinv, vx);
    }
    float xx = abs(x) - 75.0f;
    return Combine(xx, 42.0f - y);
}

inline float MapCover0(const vec3& pos) {
    float re = MapPianoBodyShapeDist(-pos.x, pos.z);
    float re_2 = abs(pos.y) - 1.0f;
    return Combine(re, re_2);
}

inline float MapBody(const vec3& pos) {
    float re = MapPianoBodyShapeDist(-pos.x, pos.z);
    float re_2 = abs(pos.y) - 15.0f;
    return Combine(re, re_2);
}

inline float MapPianoBody(const vec3& world_pos) {
    vec3 pos = world_pos + place_pos;

    // backfoot
    float re = Map2Box(pos - vec3(30.0f, 40.0f, 155.0f), vec2(15.0f, 3.2f), vec2(3.0f, 3.0f), 35.0f);

    // foot1
    float re_2 = Map2Box(pos - vec3(-67.0f, 40.0f, 12.0f), vec2(5.0f, 12.5f), vec2(3.0f, 3.0f), 35.0f);
    re = glm::min(re, re_2);
    // foot2
    re_2 = Map2Box(pos - vec3(67.0f, 40.0f, 12.0f), vec2(5.0f, 12.5f), vec2(3.0f, 3.0f), 35.0f);
    re = glm::min(re, re_2);

    // pedal box
    re_2 = MapBoxSim(pos - vec3(0.0f, 10.0f, 12.0f), vec3(14.0f, 5.0f, 5.0f));
    re = glm::min(re, re_2);
    // pedal box2
    re_2 = Map2Box(pos - vec3(0.0f, 42.5f, 14.0f), vec2(14.0f, 4.0f), vec2(5.0f, 2.0f), 35.0f);
    re = glm::min(re, re_2);

    // keyboard bottom
    re_2 = MapBoxSim(pos - vec3(0.0f, 74.5f, -9.0f), vec3(75.0f, 4.5f, 9.0f));
    re = glm::min(re, re_2);

    // keyboard two side
    re_2 = MapBoxSim(pos - vec3(68.0f, 77.0f, -9.0f), vec3(7.0f, 7.0f, 9.0f));
    re = glm::min(re, re_2);
    re_2 = MapBoxSim(pos - vec3(-70.0f, 77.0f, -9.0f), vec3(5.0f, 7.0f, 9.0f));
    re = glm::min(re, re_2);

    // open cover
    {
        vec3 pos_trans = pos + vec3(-75.0f, -99.0f, 0.0f);
        float angle = abs(fract((hinge_angle + 0.05f) * 0.05f) - 0.5f);
        float sinv = sin(angle);
        float cosv = cos(angle);
        mat2 rot = mat2(cosv, -sinv, sinv, cosv);
        vec2 pt = rot * vec2(pos_trans.x, pos_trans.y);
        pos_trans.x = pt.x; pos_trans.y = pt.y;
        pos_trans.x += 75.0f;
        re_2 = MapCover0(pos_trans);
        re = glm::min(re, re_2);
        re_2 = MapBoxSim(pos_trans - vec3(0.0f, 3.0f, 63.0f), vec3(75.0f, 1.0f, 21.0f));
        re = glm::min(re, re_2);
    }

    // frame
    {
        vec3 pos_temp = pos - vec3(0.0f, 83.0f, 21.0f);
        float x = abs(pos_temp.x) - 75.0f;
        float y = abs(pos_temp.y) - 13.0f;
        float z = abs(pos_temp.z) - 21.0f;
        z = Combine(x, z);
        if (pos_temp.z > 0.0f && pos_temp.z < 22.0f) z = x;
        z = abs(z + 4.0f) - 4.0f;
        re_2 = Combine(z, y);
        re = glm::min(re, re_2);

        pos_temp = pos - vec3(0.0f, 83.0f, 0.0f);
        y = MapPianoBodyShapeDist(-pos_temp.x, pos_temp.z);
        if (pos_temp.z > 40.0f && pos_temp.z < 44.0f) y = abs(pos_temp.x) - 75.0f;
        y = abs(y + 4.0f) - 4.0f;
        z = abs(pos_temp.y) - 13.0f;
        re_2 = Combine(y, z);
        re = glm::min(re, re_2);
    }

    return re;
}

inline float MapBodySoundboard(const vec3& world_pos) {
    vec3 pos = world_pos + place_pos - vec3(0.0f, 80.0f, -10.0f);
    float re_3 = MapPianoBodyShapeDist(-pos.x, pos.z);
    float re_2 = abs(pos.y) - 6.0f;
    float re = Combine(re_3, re_2) + 4.0f;
    return re;
}

inline float MapGoldObjs(const vec3& world_pos) {
    vec3 pos = world_pos + place_pos;

    // wheel
    float re = MapCylinder(vec3((pos - vec3(-67.0f, 3.0f, 12.0f)).x, (pos - vec3(-67.0f, 3.0f, 12.0f)).z, (pos - vec3(-67.0f, 3.0f, 12.0f)).y), 2.5f, 5.0f);
    float re_2 = MapCylinder(vec3((pos - vec3(67.0f, 3.0f, 12.0f)).z, (pos - vec3(67.0f, 3.0f, 12.0f)).x, (pos - vec3(67.0f, 3.0f, 12.0f)).y), 2.5f, 5.0f);
    re = glm::min(re, re_2);
    re_2 = MapCylinder(vec3((pos - vec3(30.0f, 3.0f, 155.0f)).x, (pos - vec3(30.0f, 3.0f, 155.0f)).z, (pos - vec3(30.0f, 3.0f, 155.0f)).y), 2.5f, 5.0f);
    re = glm::min(re, re_2);

    // pedal
    re_2 = MapBoxSim(pos - vec3(0.0f, 7.0f, 3.0f), vec3(2.0f, 0.5f, 5.0f));
    re = glm::min(re, re_2);
    re_2 = MapBoxSim(pos - vec3(9.0f, 7.0f, 3.0f), vec3(2.0f, 0.5f, 5.0f));
    re = glm::min(re, re_2);
    re_2 = MapBoxSim(pos - vec3(-9.0f, 7.0f, 3.0f), vec3(2.0f, 0.5f, 5.0f));
    re = glm::min(re, re_2);

    // inner box
    re_2 = MapBoxSim(pos - vec3(0.0f, 80.0f, 18.0f), vec3(70.0f, 10.0f, 14.0f));
    re = glm::min(re, re_2);

    // inner
    {
        vec3 pos_1 = pos - vec3(0.0f, 85.0f, 0.0f);
        float re_3 = MapPianoBodyShapeDist(-pos_1.x, pos_1.z);
        re_2 = abs(pos_1.y) - 14.0f;
        re_2 = Combine(re_3, re_2) + 10.0f;

        float sinv = sin(-0.66f);
        float cosv = cos(-0.66f);
        pos_1.x -= 40.0f;
        pos_1.z -= 30.0f;
        mat2 rot = mat2(cosv, -sinv, sinv, cosv);
        vec2 pt = rot * vec2(pos_1.x, pos_1.z);
        pos_1.x = pt.x; pos_1.z = pt.y;
        re_3 = MapBox(pos_1, vec3(110.0f, 25.0f, 60.0f));
        re_2 = Subtract(re_2, re_3);
        re_2 += 3.0f;

        // line
        re_3 = MapBoxSim(pos - vec3(-59.0f, 85.0f, 27.0f), vec3(1.0f, 10.0f, 23.0f));
        re_2 = glm::min(re_2, re_3);
        re_3 = MapBoxSim(pos - vec3(-30.0f, 85.0f, 40.0f), vec3(1.0f, 10.0f, 36.0f));
        re_2 = glm::min(re_2, re_3);
        re_3 = MapBoxSim(pos - vec3(-4.0f, 85.0f, 50.0f), vec3(1.0f, 10.0f, 46.0f));
        re_2 = glm::min(re_2, re_3);
        re_3 = MapBoxSim(pos - vec3(26.0f, 85.0f, 70.0f), vec3(1.0f, 10.0f, 66.0f));
        re_2 = glm::min(re_2, re_3);
        re_3 = MapBoxSim(pos - vec3(62.0f, 85.0f, 75.0f), vec3(1.0f, 10.0f, 71.0f));
        re_2 = glm::min(re_2, re_3);

        // hole
        re_3 = MapCylinder(pos - vec3(-42.0f, 85.0f, 60.0f), 4.0f, 10.0f);
        re_2 = Subtract(re_2, re_3);
        re_3 = MapCylinder(pos - vec3(-17.0f, 85.0f, 82.0f), 5.0f, 10.0f);
        re_2 = Subtract(re_2, re_3);
        re_3 = MapCylinder(pos - vec3(9.0f, 85.0f, 115.0f), 6.0f, 10.0f);
        re_2 = Subtract(re_2, re_3);

        re = glm::min(re, re_2);
    }

    return re;
}

inline float MapString(const vec3& world_pos) {
    vec3 pos = world_pos + place_pos - vec3(0.0f, 85.0f, 80.0f);
    float re = MapBox(world_pos + place_pos - vec3(0.0f, 85.0f, 80.0f), vec3(56.5f, 0.001f, 54.0f));

    float sinv = sin(-0.66f);
    float cosv = cos(-0.66f);
    pos.z -= 80.0f;
    mat2 rot = mat2(cosv, -sinv, sinv, cosv);
    vec2 pt = rot * vec2(pos.x, pos.z);
    pos.x = pt.x; pos.z = pt.y;
    float re_2 = MapBox(pos, vec3(130.0f, 25.0f, 60.0f));
    re = Subtract(re, re_2);

    if (re < EPSILON) {
        re_2 = (world_pos.x + 56.5f) / 113.0f * 88.0f;
        re_2 = abs(fract(re_2) - 0.5f);
        if (re_2 < 0.499f)
            re = glm::max(re, re_2 * 2.5681818181818181818181818181818f);
    }

    return re;
}

inline float MapBlackKeys(const vec3& world_pos) {
    vec3 pos = world_pos + place_pos - vec3(-2.0f, 82.75f, -5.0f);
    float re = MapBoxSim(pos, vec3(63.0f, 0.75f, 5.0f));

    if (re <= EPSILON) {
        float x = clamp(pos.x / 126.0f + 0.5f, 0.0f, 1.0f);
        if (x < 0.03175f) {
            re = glm::max(re, (0.03175f - x) * 126.0f);
        } else if (x > 0.984f) {
            re = glm::max(re, (1.0f - x) * 126.0f);
        } else {
            x = clamp(x - 0.02f, 0.0f, 1.0f);
            x = fract(x * 7.4285714285714285714285714285714f);
            if (x < 0.1f)
                re = glm::max(re, (0.1f - x) * 7.4285714285714285714285714285714f);
            else if (x > 0.8861f)
                re = glm::max(re, (x - 0.8861f) * 7.4285714285714285714285714285714f);
            else if (x > 0.16f && x < 0.2476f)
                re = glm::max(re, (0.2038f - abs(x - 0.2038f)) * 7.4285714285714285714285714285714f);
            else if (x > 0.3343f && x < 0.395f)
                re = glm::max(re, (0.36465f - abs(x - 0.36465f)) * 7.4285714285714285714285714285714f);
            else if (x > 0.4817f && x < 0.6408f)
                re = glm::max(re, (0.56125f - abs(x - 0.56125f)) * 7.4285714285714285714285714285714f);
            else if (x > 0.7452f && x < 0.8159f)
                re = glm::max(re, (0.78055f - abs(x - 0.78055f)) * 7.4285714285714285714285714285714f);
        }
    }

    return re;
}

inline float MapWhiteKeys(const vec3& world_pos) {
    vec3 pos = world_pos + place_pos - vec3(-2.0f, 80.5f, -7.5f);
    return MapBoxSim(pos, vec3(63.0f, 1.5f, 7.5f));
}

inline float Map(const vec3& world_pos) {
    float d = MapPianoBody(world_pos);
    d = glm::min(d, MapWhiteKeys(world_pos));
    d = glm::min(d, MapBlackKeys(world_pos));
    d = glm::min(d, MapBodySoundboard(world_pos));
    d = glm::min(d, MapString(world_pos));
    d = glm::min(d, MapGoldObjs(world_pos));
    return d;
}

} // namespace grandpiano_detail

inline float GrandPiano(const vec3& p_in, float /*time*/, uint32_t /*seed*/) {
    vec3 p = p_in + vec3(0.0f, 0.61f, -0.2f);
    p = p * rotationMatrix(vec3(0.0f, 1.0f, 0.0f), pi);
    const float scale = 0.008f;
    p *= 1.0f / scale;
    return grandpiano_detail::Map(p) * scale;
}

} // namespace sdf::manufactured

