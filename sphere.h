//
// Created by meidai on 18-10-14.
//

#ifndef RAYTRACER_SPHERE_H
#define RAYTRACER_SPHERE_H

#include "hitable.h"

class sphere : public hitable {
public:
    vec3 center;
    float radius;
    material *mat_ptr;

    sphere() {}

    sphere(vec3 cen, float r, material *mat_ptr) : center(cen), radius(r), mat_ptr(mat_ptr) {}

    bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const;

    bool bounding_box(float t0, float t1, aabb &box) const;
};

bool sphere::hit(const ray &r, float t_min, float t_max, hit_record &rec) const {
    vec3 oc = r.origin() - center;
    float a = dot(r.direction(), r.direction());
    float b = 2.0f * dot(oc, r.direction());
    float c = dot(oc, oc) - radius * radius;
    float discriminant = b * b - 4 * a * c;
    if (discriminant > 0) {
        float temp = (-b - sqrt(discriminant)) / (2 * a);
        if (temp > t_min && temp < t_max) {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center) / radius;
            rec.mat_ptr = mat_ptr;
            return true;
        }
        temp = (-b + sqrt(discriminant)) / (2 * a);
        if (temp > t_min && temp < t_max) {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center) / radius;
            rec.mat_ptr = mat_ptr;
            return true;
        }
    }
    return false;
}

bool sphere::bounding_box(float t0, float t1, aabb &box) const {
    box = aabb(center - vec3(radius, radius, radius), center + vec3(radius, radius, radius));
    return true;
}

#endif //RAYTRACER_SPHERE_H
