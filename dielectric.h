//
// Created by wly on 2018/10/15.
//

#ifndef RAYTRACER_DIELECTRIC_H
#define RAYTRACER_DIELECTRIC_H

#include "material.h"

class dielectric:public material{
public:
    float ref_idx;

    dielectric(float ri):ref_idx(ri){}
    bool scatter(const ray &r_in, const hit_record &rec, vec3 &attenuation, ray &scattered) const override{
        vec3 outward_normal;
        vec3 reflected = reflect(r_in.direction(),rec.normal);
        float ni_over_nt;
        attenuation = vec3(1.0,1.0,1.0);
        vec3 refracted;
        if(dot(r_in.direction(),rec.normal)>0){
            outward_normal = -rec.normal;
            ni_over_nt = ref_idx;
        }else{
            outward_normal = rec.normal;
            ni_over_nt = 1.0f/ref_idx;
        }
        if(refract(r_in.direction(),outward_normal,ni_over_nt,refracted)){
            scattered = ray(rec.p,refracted);
        }else{
            scattered = ray(rec.p,refracted);
            return false;
        }
        return true;
    }
};

#endif //RAYTRACER_DIELECTRIC_H
