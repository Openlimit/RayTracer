#include <iostream>
#include <fstream>
#include <cfloat>
#include <vector>

#include "sphere.h"
#include "hitable_list.h"
#include "camera.h"
#include "bvh.h"
#include "rectangle.h"
#include "material.h"
#include "triangle.h"
#include "obj_load.h"
#include "Light.h"

using namespace std;

inline vec3 de_nan(const vec3 &c) {
    vec3 temp = c;
    if (!(temp[0] == temp[0])) temp[0] = 0;
    if (!(temp[1] == temp[1])) temp[1] = 0;
    if (!(temp[2] == temp[2])) temp[2] = 0;
    return temp;
}

void room(int nx, int ny, camera **cam, hitable **world, vector<Light *> &light_list) {
    vec3 lookfrom(0, 0, 4);
    vec3 lookat(0, 0, 0);
    vec3 up(0, 1, 0);
    float dist_to_focus = 10;
    float aperture = 0.0;
    float vfov = 50.0;
    *cam = new camera(lookfrom, lookat, up, vfov, float(nx) / float(ny), aperture, dist_to_focus, 0.0, 1.0);

    mtl m;
    m.ka = vec3(50, 50, 40);
    material *phong = new Phong(m);
    hitable *light_shape = new sphere(vec3(0, 1.589, -1.274), 0.2, phong);
    Light *light = new Light(light_shape, m.ka);
    light_list.emplace_back(light);

    vector<hitable *> mesh;
    load_obj("../objects/", "room.obj", mesh);
    mesh.emplace_back(light_shape);
    *world = new bvh(mesh.data(), mesh.size(), 0, FLT_MAX);
}

void cup(int nx, int ny, camera **cam, hitable **world, vector<Light *> &light_list) {
    vec3 lookfrom(0.0, 0.64, 0.52);
    vec3 lookat(0.0, 0.40, 0.3);
    vec3 up(0, 1, 0);
    float dist_to_focus = 1;
    float aperture = 0.0;
    float vfov = 60.0;
    *cam = new camera(lookfrom, lookat, up, vfov, float(nx) / float(ny), aperture, dist_to_focus, 0.0, 1.0);

    mtl m;
    m.ka = vec3(40, 40, 40);
    material *phong = new Phong(m);
    hitable *light_shape = new yz_rect(1.0246, 2.0246, -0.5, 0.5, -2.758771896, phong);
    Light *light = new Light(light_shape, m.ka);
    light_list.emplace_back(light);

    vector<hitable *> mesh;
    load_obj("../objects/", "cup.obj", mesh);
    mesh.emplace_back(light_shape);
    *world = new bvh(mesh.data(), mesh.size(), 0, FLT_MAX);
}

void VeachMIS(int nx, int ny, camera **cam, hitable **world, vector<Light *> &light_list) {
    vec3 lookfrom(0.0, 2.0, 15.0);
    vec3 lookat(0.0, 1.69521, 14.0476);
    vec3 up(0.0, 0.952421, -0.304787);
    float dist_to_focus = 1;
    float aperture = 0.0;
    float vfov = 40.0;
    *cam = new camera(lookfrom, lookat, up, vfov, float(nx) / float(ny), aperture, dist_to_focus, 0.0, 1.0);

    mtl m1;
    m1.ka = vec3(800, 800, 800);
    hitable *light_shape1 = new sphere(vec3(-10, 10, 4), 0.5, new Phong(m1));
    Light *light1 = new Light(light_shape1, m1.ka);
    light_list.emplace_back(light1);

    mtl m2;
    m2.ka = vec3(901.803, 901.803, 901.803);
    hitable *light_shape2 = new sphere(vec3(3.75, 0, 0), 0.033, new Phong(m2));
    Light *light2 = new Light(light_shape2, m2.ka);
    light_list.emplace_back(light2);

    mtl m3;
    m3.ka = vec3(100, 100, 100);
    hitable *light_shape3 = new sphere(vec3(1.25, 0, 0), 0.1, new Phong(m3));
    Light *light3 = new Light(light_shape3, m3.ka);
    light_list.emplace_back(light3);

    mtl m4;
    m4.ka = vec3(11.1111, 11.1111, 11.1111);
    hitable *light_shape4 = new sphere(vec3(-1.25, 0, 0), 0.3, new Phong(m4));
    Light *light4 = new Light(light_shape4, m4.ka);
    light_list.emplace_back(light4);

    mtl m5;
    m5.ka = vec3(1.23457, 1.23457, 1.23457);
    hitable *light_shape5 = new sphere(vec3(-3.75, 0, 0), 0.9, new Phong(m5));
    Light *light5 = new Light(light_shape5, m5.ka);
    light_list.emplace_back(light5);

    vector<hitable *> mesh;
    load_obj("../objects/", "VeachMIS.obj", mesh);
    mesh.emplace_back(light_shape1);
    mesh.emplace_back(light_shape2);
    mesh.emplace_back(light_shape3);
    mesh.emplace_back(light_shape4);
    mesh.emplace_back(light_shape5);
    *world = new bvh(mesh.data(), mesh.size(), 0, FLT_MAX);
}

vec3 radiance(const ray &r, hitable *world, vector<Light *> &light_list, int depth) {
    if (depth > 10)
        return vec3(0, 0, 0);
    hit_record rec;
    if (world->hit(r, 0.001, FLT_MAX, rec)) {
        if (rec.mat_ptr->is_light())
            return rec.mat_ptr->emitted(r, rec);
        vec3 color_from_refraction(0);
        vec3 color_from_specular(0);
        vec3 color_from_recursive_indirect_light(0);
        vec3 color_from_light_sources(0);

        scatter_record refract_sr;
        if (rec.mat_ptr->refraction(r, rec, refract_sr)) {
            color_from_refraction = refract_sr.attenuation * radiance(refract_sr.r_out, world, light_list, depth + 1);
        }

        scatter_record specular_sr;
        if (rec.mat_ptr->specular(r, rec, specular_sr)) {
            color_from_specular = specular_sr.attenuation * radiance(specular_sr.r_out, world, light_list, depth + 1);
        }

        scatter_record diffuse_sr;
        if (rec.mat_ptr->diffuse(r, rec, diffuse_sr)) {
            for (int i = 0; i < light_list.size(); ++i) {
                Light *light = light_list[i];
                ray light_ray = ray(rec.p, light->get_direction(rec.p));
                float cos_theta = dot(light_ray.direction(), rec.normal);
                if (cos_theta > 0 && light->trace(light_ray, world)) {
                    color_from_light_sources +=
                            diffuse_sr.attenuation * cos_theta *
                            light->get_intensity(light_ray) / (light->get_pdf(light_ray) * M_PI);
                }
            }

            color_from_recursive_indirect_light =
                    diffuse_sr.attenuation * radiance(diffuse_sr.r_out, world, light_list, depth + 1);
        }

        return color_from_refraction + color_from_specular + color_from_light_sources +
               color_from_recursive_indirect_light;
    } else {
        return vec3(0);
    }
}

int main() {
    ofstream outfile("hellograph.ppm");
    int nx = 100, ny = 100, ns = 1000;
    outfile << "P3\n" << nx << " " << ny << "\n255\n";

    camera *cam;
    hitable *world;
    vector<Light *> light_list;
    room(nx, ny, &cam, &world, light_list);
//    cup(nx, ny, &cam, &world, light_list);
//    VeachMIS(nx, ny, &cam, &world, light_list);

    for (int j = ny - 1; j >= 0; --j) {
        cout << j << endl;
        for (int i = 0; i < nx; ++i) {
            vec3 col(0, 0, 0);
            for (int s = 0; s < ns; ++s) {
                float u = float(i + drand48()) / nx;
                float v = float(j + drand48()) / ny;
                ray r = cam->get_ray(u, v);
                col += de_nan(radiance(r, world, light_list, 0));
            }

            col /= float(ns);
            col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));//gamma corrected
            int ir = int(ffmin(255.99 * col[0], 255));
            int ig = int(ffmin(255.99 * col[1], 255));
            int ib = int(ffmin(255.99 * col[2], 255));
            outfile << ir << " " << ig << " " << ib << "\n";
        }
    }
    outfile.close();

    return 0;
}