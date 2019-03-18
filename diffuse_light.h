//
// Created by 王惠 on 2019/3/18.
//

#ifndef RAYTRACER_DIFFUSE_LIGHT_H
#define RAYTRACER_DIFFUSE_LIGHT_H

#include "material.h"
#include "texture.h"

class diffuse_light : public material {
public:
    texture *emit;

    diffuse_light(texture *a) : emit(a) {}

    virtual bool scatter(const ray &r_in, const hit_record &rec, vec3 &attenuation, ray &scattered) const {
        return false;
    }

    virtual vec3 emitted(float u, float v, const vec3 &p) const {
        return emit->value(u, v, p);
    }

};

#endif //RAYTRACER_DIFFUSE_LIGHT_H
