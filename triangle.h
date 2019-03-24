//
// Created by 王惠 on 2019/3/21.
//

#ifndef RAYTRACER_TRIANGLE_H
#define RAYTRACER_TRIANGLE_H

#include "hitable.h"

// Determine whether a ray intersect with a triangle
// Parameters
// orig: origin of the ray
// dir: direction of the ray
// v0, v1, v2: vertices of triangle
// t(out): weight of the intersection for the ray
// u(out), v(out): barycentric coordinate of intersection

bool IntersectTriangle(const vec3 &orig, const vec3 &dir,
                       const vec3 &v0, const vec3 &v1, const vec3 &v2,
                       float *t, float *u, float *v) {
    // E1
    vec3 E1 = v1 - v0;
    // E2
    vec3 E2 = v2 - v0;
    // P
    vec3 P = cross(dir, E2);

    // determinant
    float det = dot(E1, P);

    // keep det > 0, modify T accordingly
    vec3 T;
    if (det > 0) {
        T = orig - v0;
    } else {
        T = v0 - orig;
        det = -det;
    }

    // If determinant is near zero, ray lies in plane of triangle
    if (det < 0.0001f)
        return false;

    // Calculate u and make sure u <= 1
    *u = dot(T, P);
    if (*u < 0.0f || *u > det)
        return false;

    // Q
    vec3 Q = cross(T, E1);

    // Calculate v and make sure u + v <= 1
    *v = dot(dir, Q);
    if (*v < 0.0f || *u + *v > det)
        return false;

    // Calculate t, scale parameters, ray intersects triangle
    *t = dot(E2, Q);

    float fInvDet = 1.0f / det;
    *t *= fInvDet;
    *u *= fInvDet;
    *v *= fInvDet;

    return true;
}

class triangle : public hitable {
public:
    vec3 vertex[3];
    vec3 vn[3];
    vec3 normal;
    material *mat_ptr;
    vec3 _min;
    vec3 _max;

    triangle(const vec3 &v0, const vec3 &v1, const vec3 &v2, material *mtp) {
        vertex[0] = v0;
        vertex[1] = v1;
        vertex[2] = v2;
        mat_ptr = mtp;

        vec3 v0v1 = v1 - v0;
        vec3 v0v2 = v2 - v0;
        vec3 N = cross(v0v1, v0v2);
        normal = unit_vector(N);

        float min_x = FLT_MAX;
        float min_y = FLT_MAX;
        float min_z = FLT_MAX;
        float max_x = FLT_MIN;
        float max_y = FLT_MIN;
        float max_z = FLT_MIN;
        for (int i = 0; i < 3; ++i) {
            min_x = ffmin(vertex[i][0], min_x);
            min_y = ffmin(vertex[i][1], min_y);
            min_z = ffmin(vertex[i][2], min_z);
            max_x = ffmax(vertex[i][0], max_x);
            max_y = ffmax(vertex[i][1], max_y);
            max_z = ffmax(vertex[i][2], max_z);
        }
        _min = vec3(min_x, min_y, min_z);
        _max = vec3(max_x, max_y, max_z);
    }

    triangle(const vec3 &v0, const vec3 &v1, const vec3 &v2, const vec3 &vn0, const vec3 &vn1, const vec3 &vn2,
             material *mtp) {
        vertex[0] = v0;
        vertex[1] = v1;
        vertex[2] = v2;
        vn[0] = vn0;
        vn[1] = vn1;
        vn[2] = vn2;
        mat_ptr = mtp;

        normal = unit_vector((vn0 + vn1 + vn2) / 3);

        float min_x = FLT_MAX;
        float min_y = FLT_MAX;
        float min_z = FLT_MAX;
        float max_x = FLT_MIN;
        float max_y = FLT_MIN;
        float max_z = FLT_MIN;
        for (int i = 0; i < 3; ++i) {
            min_x = ffmin(vertex[i][0], min_x);
            min_y = ffmin(vertex[i][1], min_y);
            min_z = ffmin(vertex[i][2], min_z);
            max_x = ffmax(vertex[i][0], max_x);
            max_y = ffmax(vertex[i][1], max_y);
            max_z = ffmax(vertex[i][2], max_z);
        }
        _min = vec3(min_x, min_y, min_z);
        _max = vec3(max_x, max_y, max_z);
    }

    virtual bool bounding_box(float t0, float t1, aabb &box) const {
        vec3 margin(0.0001, 0.0001, 0.0001);
        vec3 mmin = _min - margin;
        vec3 mmax = _max + margin;
        box = aabb(mmin, mmax);
        return true;
    }

    virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const {
        if (dot(normal, r.direction()) >= 0)
            return false;

        float t, u, v;
        bool is_hit = IntersectTriangle(r.origin(), r.direction(), vertex[0], vertex[1], vertex[2], &t, &u, &v);
        is_hit = is_hit && t >= t_min && t <= t_max;
        if (is_hit) {
            rec.normal = normal;
            rec.mat_ptr = mat_ptr;
            rec.p = r.point_at_parameter(t);
            rec.t = t;
        }
        return is_hit;
    }
};

#endif //RAYTRACER_TRIANGLE_H
