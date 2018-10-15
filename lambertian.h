//
// Created by meidai on 18-10-14.
//

#ifndef RAYTRACER_LAMBERTIAN_H
#define RAYTRACER_LAMBERTIAN_H

#include "material.h"

class lambertian : public material {
public:
    vec3 albedo;

    lambertian(const vec3 &a) : albedo(a) {}

    bool scatter(const ray &r_in, const hit_record &rec, vec3 &attenuation, ray &scattered) const override {
        vec3 target = rec.p + rec.normal + random_unit_sphere();
        scattered = ray(rec.p, target - rec.p);
        attenuation = albedo;
        return true;
    }
};

#endif //RAYTRACER_LAMBERTIAN_H
