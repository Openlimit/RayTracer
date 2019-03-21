//
// Created by meidai on 18-10-14.
//

#ifndef RAYTRACER_MATERIAL_H
#define RAYTRACER_MATERIAL_H

#include "ray.h"
#include "hitable.h"
#include "texture.h"
#include "onb.h"
#include "pdf.h"

float schlick(float cosine, float ref_idx) {
    float r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
}

bool refract(const vec3 &v, const vec3 &n, float ni_over_nt, vec3 &refracted) {
    vec3 uv = unit_vector(v);
    float dt = dot(uv, n);
    float discriminant = 1.0 - ni_over_nt * ni_over_nt * (1 - dt * dt);
    if (discriminant > 0) {
        refracted = ni_over_nt * (uv - n * dt) - n * sqrt(discriminant);
        return true;
    } else
        return false;
}


vec3 reflect(const vec3 &v, const vec3 &n) {
    return v - 2 * dot(v, n) * n;
}

struct scatter_record {
    ray specular_ray;
    bool is_specular;
    vec3 attenuation;
    pdf *pdf_ptr;
};

class material {
public:
    virtual bool scatter(const ray &r_in, const hit_record &rec, scatter_record &srec) const {
        return false;
    }

    virtual float scattering_pdf(const ray &r_in, const hit_record &rec, const ray &scattered) const {
        return false;
    }

    virtual vec3 emitted(const ray &r_in, const hit_record &rec) const {
        return vec3(0, 0, 0);
    }


};

class lambertian : public material {
public:
    lambertian(texture *a, pdf *p) : albedo(a), pdf_ptr(p) {}

    float scattering_pdf(const ray &r_in, const hit_record &rec, const ray &scattered) const {
        float cosine = dot(rec.normal, unit_vector(scattered.direction()));
        if (cosine < 0)
            cosine = 0;
        return cosine / M_PI;
    }

    bool scatter(const ray &r_in, const hit_record &rec, scatter_record &srec) const {
        srec.attenuation = albedo->value(rec.u, rec.v, rec.p);
        srec.pdf_ptr = pdf_ptr;
        return true;
    }

    texture *albedo;
    pdf *pdf_ptr;
};

class diffuse_light : public material {
public:
    texture *emit;

    diffuse_light(texture *a) : emit(a) {}

    virtual vec3 emitted(const ray &r_in, const hit_record &rec) const {
        if (dot(rec.normal, r_in.direction()) > 0.0)
            return emit->value(rec.u, rec.v, rec.p);
        else
            return vec3(0, 0, 0);
    }

};

#endif //RAYTRACER_MATERIAL_H
