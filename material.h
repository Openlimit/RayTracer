//
// Created by meidai on 18-10-14.
//

#ifndef RAYTRACER_MATERIAL_H
#define RAYTRACER_MATERIAL_H

#include "ray.h"
#include "hitable.h"
#include "onb.h"

inline vec3 random_direction(float &r1, float &r2) {
    r1 = drand48();
    r2 = drand48();
    float sinTheta = sqrtf(1 - r1 * r1);
    float phi = 2 * M_PI * r2;
    float x = sinTheta * cosf(phi);
    float z = sinTheta * sinf(phi);
    return vec3(x, r1, z);
}

vec3 random_in_unit_sphere() {
    vec3 p;
    do {
        p = 2.0 * vec3(drand48(), drand48(), drand48()) - vec3(1, 1, 1);
    } while (dot(p, p) >= 1.0);
    return p;
}

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
    ray r_out;
    vec3 attenuation;
    float r1;
    float r2;
};

class material {
public:
    virtual vec3 emitted(const ray &r_in, const hit_record &rec) const {
        return vec3(0);
    }

    virtual bool refraction(const ray &r_in, const hit_record &rec, scatter_record &sr) const {
        return false;
    }

    virtual bool specular(const ray &r_in, const hit_record &rec, scatter_record &sr) const {
        return false;
    }

    virtual bool diffuse(const ray &r_in, const hit_record &rec, scatter_record &sr) const {
        return false;
    }

    virtual bool is_light() const { return false; }

};

struct mtl {
    float illum;
    float ni;
    float ns;
    vec3 kd;
    vec3 ka;
    vec3 tf;
    vec3 ks;

    mtl() {
        ka = kd = ks = vec3(0);
        ni = 1;
    }
};

class Phong : public material {
public:
    mtl phong_mtl;

    Phong(mtl &m) : phong_mtl(m) {}

    virtual vec3 emitted(const ray &r_in, const hit_record &rec) const {
        return phong_mtl.ka;
    }

    virtual bool refraction(const ray &r_in, const hit_record &rec, scatter_record &sr) const {
        if (fabs(phong_mtl.ni - 1) < 1e-6)
            return false;

        vec3 outward_normal;
        vec3 refracted;
        float ni_over_nt;
        float reflect_prob;
        float cosine;
        if (dot(r_in.direction(), rec.normal) > 0) {
            outward_normal = -rec.normal;
            ni_over_nt = phong_mtl.ni;
        } else {
            outward_normal = rec.normal;
            ni_over_nt = 1 / phong_mtl.ni;
        }
        if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted)) {
            sr.r_out = ray(rec.p, refracted);
            sr.attenuation = vec3(1);
            return true;
        } else {
            return false;
        }
    }

    virtual bool specular(const ray &r_in, const hit_record &rec, scatter_record &sr) const {
        if (phong_mtl.ks[0] <= 0 && phong_mtl.ks[1] <= 0 && phong_mtl.ks[2] <= 0)
            return false;
        if (dot(r_in.direction(), rec.normal) > 0)
            return false;

        float fuzz = 0.1;
        vec3 reflected = reflect(r_in.direction(), rec.normal);
        sr.r_out = ray(rec.p, reflected + fuzz * random_in_unit_sphere());
        vec3 out_reflected = reflect(-sr.r_out.direction(), rec.normal);
        float p = pow(std::max(0.f, dot(-r_in.direction(), out_reflected)), phong_mtl.ns);
        sr.attenuation = phong_mtl.ks * p;
        return true;
    }

    virtual bool diffuse(const ray &r_in, const hit_record &rec, scatter_record &sr) const {
        if (phong_mtl.kd[0] <= 0 && phong_mtl.kd[1] <= 0 && phong_mtl.kd[2] <= 0)
            return false;
        if (dot(r_in.direction(), rec.normal) > 0)
            return false;

        onb uvw;
        uvw.build_from_w(rec.normal);
        vec3 dir = uvw.local(random_direction(sr.r1, sr.r2));
        sr.r_out = ray(rec.p, dir);
        sr.attenuation = phong_mtl.kd;
        return true;
    }

    virtual bool is_light() const {
        return phong_mtl.ka[0] > 0 || phong_mtl.ka[1] > 0 || phong_mtl.ka[2] > 0;
    }
};

#endif //RAYTRACER_MATERIAL_H
