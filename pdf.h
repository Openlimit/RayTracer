//
// Created by 王惠 on 2019/3/21.
//

#ifndef RAYTRACER_PDF_H
#define RAYTRACER_PDF_H

#include "onb.h"
#include "hitable.h"

inline vec3 random_cosine_direction() {
    float r1 = drand48();
    float r2 = drand48();
    float z = sqrt(1 - r2);
    float phi = 2 * M_PI * r1;
    float x = cos(phi) * 2 * sqrt(r2);
    float y = sin(phi) * 2 * sqrt(r2);
    return vec3(x, y, z);
}

inline vec3 random_to_sphere(float radius, float distance_squared) {
    float r1 = drand48();
    float r2 = drand48();
    float z = 1 + r2 * (sqrt(1 - radius * radius / distance_squared) - 1);
    float phi = 2 * M_PI * r1;
    float x = cos(phi) * sqrt(1 - z * z);
    float y = sin(phi) * sqrt(1 - z * z);
    return vec3(x, y, z);
}


vec3 random_in_unit_sphere() {
    vec3 p;
    do {
        p = 2.0 * vec3(drand48(), drand48(), drand48()) - vec3(1, 1, 1);
    } while (dot(p, p) >= 1.0);
    return p;
}


class pdf {
public:
    virtual float value(const hit_record &rec, const vec3 &direction) const = 0;

    virtual vec3 generate(const hit_record &rec) const = 0;
};


class cosine_pdf : public pdf {
public:
    virtual float value(const hit_record &rec, const vec3 &direction) const {
        float cosine = dot(unit_vector(direction), rec.normal);
        if (cosine > 0)
            return cosine / M_PI;
        else
            return 0;
    }

    virtual vec3 generate(const hit_record &rec) const {
        onb uvw;
        uvw.build_from_w(rec.normal);
        return uvw.local(random_cosine_direction());
    }
};

class hitable_pdf : public pdf {
public:
    hitable_pdf(hitable *p) : ptr(p) {}

    virtual float value(const hit_record &rec, const vec3 &direction) const {
        return ptr->pdf_value(rec.p, direction);
    }

    virtual vec3 generate(const hit_record &rec) const {
        return ptr->random(rec.p);
    }

    hitable *ptr;
};

class mixture_pdf : public pdf {
public:
    mixture_pdf(pdf *p0, pdf *p1) {
        p[0] = p0;
        p[1] = p1;
    }

    virtual float value(const hit_record &rec, const vec3 &direction) const {
        return 0.5 * p[0]->value(rec, direction) + 0.5 * p[1]->value(rec, direction);
    }

    virtual vec3 generate(const hit_record &rec) const {
        if (drand48() < 0.5)
            return p[0]->generate(rec);
        else
            return p[1]->generate(rec);
    }

    pdf *p[2];
};

#endif //RAYTRACER_PDF_H
