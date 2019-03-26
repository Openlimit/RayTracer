//
// Created by meidai on 18-10-14.
//

#ifndef RAYTRACER_VEC3_H
#define RAYTRACER_VEC3_H

#include <math.h>
#include <stdlib.h>
#include <iostream>

#ifdef _WIN32

#include "drand48.h"

#endif

class vec3i {
public:
    int e[3];

    vec3i(int e0, int e1, int e2) {
        e[0] = e0;
        e[1] = e1;
        e[2] = e2;
    }

    vec3i(const vec3i &v) {
        e[0] = v.e[0];
        e[1] = v.e[1];
        e[2] = v.e[2];
    }

    inline int x() const { return e[0]; }

    inline int y() const { return e[1]; }

    inline int z() const { return e[2]; }

    inline const vec3i &operator+() const { return *this; }

    inline vec3i operator-() const { return vec3i(-e[0], -e[1], -e[2]); }

    inline float operator[](int i) const { return e[i]; }

    inline int &operator[](int i) { return e[i]; }

    inline int squared_length() const {
        return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
    }
};

class vec3 {
public:
    float e[3];

    vec3() {}

    vec3(float e0, float e1, float e2) {
        e[0] = e0;
        e[1] = e1;
        e[2] = e2;
    }

    vec3(const vec3 &v) {
        e[0] = v.e[0];
        e[1] = v.e[1];
        e[2] = v.e[2];
    }

    vec3(float v) {
        e[0] = e[1] = e[2] = v;
    }

    inline float x() const { return e[0]; }

    inline float y() const { return e[1]; }

    inline float z() const { return e[2]; }

    inline float r() const { return e[0]; }

    inline float g() const { return e[1]; }

    inline float b() const { return e[2]; }

    inline const vec3 &operator+() const { return *this; }

    inline vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }

    inline float operator[](int i) const { return e[i]; }

    inline float &operator[](int i) { return e[i]; }

    inline vec3 &operator+=(const vec3 &v);

    inline vec3 &operator-=(const vec3 &v);

    inline vec3 &operator*=(const vec3 &v);

    inline vec3 &operator/=(const vec3 &v);

    inline vec3 &operator*=(const float t);

    inline vec3 &operator/=(const float t);

    inline float length() const {
        return sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]);
    }

    inline float squared_length() const {
        return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
    }

    inline void make_unit_vector();
};

inline std::istream &operator>>(std::istream &is, vec3 &t) {
    is >> t.e[0] >> t.e[1] >> t.e[2];
    return is;
}

inline std::ostream &operator>>(std::ostream &os, vec3 &t) {
    os << t.e[0] << "" << t.e[1] << "" << t.e[2];
    return os;
}

inline void vec3::make_unit_vector() {
    float k = 1.0 / sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]);
    e[0] *= k;
    e[1] *= k;
    e[2] *= k;
}

inline vec3 operator+(const vec3 &v1, const vec3 &v2) {
    return vec3(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1], v1.e[2] + v2.e[2]);
}

inline vec3 operator-(const vec3 &v1, const vec3 &v2) {
    return vec3(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1], v1.e[2] - v2.e[2]);
}

inline vec3 operator*(const vec3 &v1, const vec3 &v2) {
    return vec3(v1.e[0] * v2.e[0], v1.e[1] * v2.e[1], v1.e[2] * v2.e[2]);
}

inline vec3 operator/(const vec3 &v1, const vec3 &v2) {
    return vec3(v1.e[0] / v2.e[0], v1.e[1] / v2.e[1], v1.e[2] / v2.e[2]);
}

inline vec3 operator*(float t, const vec3 &v) {
    return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

inline vec3 operator/(const vec3 &v, float t) {
    return vec3(v.e[0] / t, v.e[1] / t, v.e[2] / t);
}

inline vec3 operator*(const vec3 &v, float t) {
    return vec3(v.e[0] * t, v.e[1] * t, v.e[2] * t);
}

inline float dot(const vec3 &v1, const vec3 &v2) {
    return v1.e[0] * v2.e[0] + v1.e[1] * v2.e[1] + v1.e[2] * v2.e[2];
}

inline vec3 cross(const vec3 &v1, const vec3 &v2) {
    return vec3(v1.e[1] * v2.e[2] - v1.e[2] * v2.e[1], -(v1.e[0] * v2.e[2] - v1.e[2] * v2.e[0]),
                v1.e[0] * v2.e[1] - v1.e[1] * v2.e[0]);
}

inline vec3 &vec3::operator+=(const vec3 &v) {
    e[0] += v[0];
    e[1] += v[1];
    e[2] += v[2];
    return *this;
}

inline vec3 &vec3::operator-=(const vec3 &v) {
    e[0] -= v[0];
    e[1] -= v[1];
    e[2] -= v[2];
    return *this;
}

inline vec3 &vec3::operator*=(const vec3 &v) {
    e[0] *= v[0];
    e[1] *= v[1];
    e[2] *= v[2];
    return *this;
}

inline vec3 &vec3::operator/=(const vec3 &v) {
    e[0] /= v[0];
    e[1] /= v[1];
    e[2] /= v[2];
    return *this;
}

inline vec3 &vec3::operator*=(const float t) {
    e[0] *= t;
    e[1] *= t;
    e[2] *= t;
    return *this;
}

inline vec3 &vec3::operator/=(const float t) {
    e[0] /= t;
    e[1] /= t;
    e[2] /= t;
    return *this;
}

inline vec3 unit_vector(vec3 v) {
    return v / v.length();
}

vec3 random_in_unit_disk() {
    vec3 p;
    do {
        p = 2.0 * vec3(drand48(), drand48(), 0) - vec3(1, 1, 0);
    } while (dot(p, p) >= 1.0);
    return p;
}

#endif //RAYTRACER_VEC3_H
