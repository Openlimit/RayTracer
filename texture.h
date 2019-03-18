//
// Created by 王惠 on 2019/3/17.
//

#ifndef RAYTRACER_TEXTURE_H
#define RAYTRACER_TEXTURE_H

#include "vec3.h"

class texture {
public:
    virtual vec3 value(float u, float v, const vec3 &p) const = 0;
};

class constant_texture : public texture {
public:
    constant_texture() = default;

    constant_texture(const vec3 &c) : color(c) {}

    virtual vec3 value(float u, float v, const vec3 &p) const {
        return color;
    }

    vec3 color;
};

class checker_texture : public texture {
public:
    texture *odd;
    texture *even;

    checker_texture() = default;

    checker_texture(texture *t0, texture *t1) : even(t0), odd(t1) {}

    virtual vec3 value(float u, float v, const vec3 &p) const {
        float sines = sin(10 * p.x()) * sin(10 * p.y()) * sin(10 * p.z());
        if (sines < 0)
            return odd->value(u, v, p);
        else
            return even->value(u, v, p);
    }
};

#endif //RAYTRACER_TEXTURE_H
