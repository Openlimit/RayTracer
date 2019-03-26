//
// Created by 王惠 on 2019/3/26.
//

#ifndef RAYTRACER_LIGHT_H
#define RAYTRACER_LIGHT_H

#include "hitable.h"

class Light {
public:
    hitable *shape;
    vec3 le;

    Light(hitable *s, const vec3 &l) : shape(s), le(l) {}

    vec3 get_intensity(const ray &r) {
        return le;
    }

    vec3 get_direction(const vec3 &p) const {
        return shape->random(p);
    };

    float get_pdf(const ray &r) const {
        return shape->pdf_value(r.origin(), r.direction());
    }

    bool trace(const ray &r, hitable *world) {
        hit_record rec;
        if (shape->hit(r, 0.001, FLT_MAX, rec)) {
            hit_record wrec;
            return !world->hit(r, 0.001, rec.t, wrec);
        } else {
            return false;
        }
    }
};

#endif //RAYTRACER_LIGHT_H
