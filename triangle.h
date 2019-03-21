//
// Created by 王惠 on 2019/3/21.
//

#ifndef RAYTRACER_TRIANGLE_H
#define RAYTRACER_TRIANGLE_H

#include "hitable.h"


class triangle : public hitable {
public:
    vec3 vertex[3];
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

        if (min_z > 0 && dot(normal, vec3(0, 0, 1)) < 0)
            normal = -normal;
        if (max_z < 0 && dot(normal, vec3(0, 0, 1)) > 0)
            normal = -normal;
    }

    virtual bool bounding_box(float t0, float t1, aabb &box) const {
        vec3 margin(0.0001, 0.0001, 0.0001);
        vec3 mmin = _min - margin;
        vec3 mmax = _max + margin;
        box = aabb(mmin, mmax);
        return true;
    }

    virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const {
        vec3 orig = r.origin();
        vec3 dir = r.direction();
        vec3 v0 = vertex[0];
        vec3 v1 = vertex[1];
        vec3 v2 = vertex[2];

        // compute plane's normal
        vec3 N = normal;
        rec.normal = normal;

        // Step 1: finding P

        // check if ray and plane are parallel ?
        float NdotRayDirection = dot(N, dir);
        if (fabs(NdotRayDirection) < 1e-6) // almost 0
            return false; // they are parallel so they don't intersect ! 

        // compute d parameter using equation 2
        float d = dot(N, v0);

        // compute t (equation 3)
        float t = -(dot(N, orig) + d) / NdotRayDirection;
        // check if the triangle is in behind the ray
        if (t < t_min || t > t_max) return false; // the triangle is behind
        rec.t = t;

        // compute the intersection point using equation 1
        vec3 P = orig + t * dir;
        rec.p = P;

        // Step 2: inside-outside test
        vec3 C; // vector perpendicular to triangle's plane 

        // edge 0
        vec3 edge0 = v1 - v0;
        vec3 vp0 = P - v0;
        C = cross(edge0, vp0);
        if (dot(N, C) < 0) return false; // P is on the right side

        // edge 1
        vec3 edge1 = v2 - v1;
        vec3 vp1 = P - v1;
        C = cross(edge1, vp1);
        if (dot(N, C) < 0) return false; // P is on the right side

        // edge 2
        vec3 edge2 = v0 - v2;
        vec3 vp2 = P - v2;
        C = cross(edge2, vp2);
        if (dot(N, C) < 0) return false; // P is on the right side;

        rec.mat_ptr = mat_ptr;
        return true; // this ray hits the triangle 
    }
};

#endif //RAYTRACER_TRIANGLE_H
