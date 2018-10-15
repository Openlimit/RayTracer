//
// Created by meidai on 18-10-14.
//

#ifndef RAYTRACER_CAMERA_H
#define RAYTRACER_CAMERA_H

#include "ray.h"

class camera {
public:
    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
    vec3 origin;

    camera() {
        lower_left_corner = vec3(-2.0f, -1.0f, -1.0f);
        horizontal = vec3(4.0, 0, 0);
        vertical = vec3(0, 2.0, 0);
        origin = vec3(0, 0, 0);
    }

    ray get_ray(float u, float v) { return ray(origin, lower_left_corner + u * horizontal + v * vertical); }

};

#endif //RAYTRACER_CAMERA_H
