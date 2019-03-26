//
// Created by meidai on 18-10-14.
//

#ifndef RAYTRACER_RAY_H
#define RAYTRACER_RAY_H

#include "vec3.h"

class ray {
public:
    vec3 A;
    vec3 B;
    float _time;

    ray() {}

    ray(const vec3 &a, const vec3 &b, float ti = 0.0) {
        A = a;
        B = unit_vector(b);
        _time = ti;
    }

    vec3 origin() const { return A; }

    vec3 direction() const { return B; }

    float time() const { return _time; }

    vec3 point_at_parameter(float t) const { return A + t * B; }
};

#endif //RAYTRACER_RAY_H
