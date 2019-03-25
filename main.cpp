#include <iostream>
#include <fstream>
#include <cfloat>
#include <vector>

#include "sphere.h"
#include "hitable_list.h"
#include "camera.h"
#include "bvh.h"
#include "texture.h"
#include "rectangle.h"
#include "box.h"
#include "material.h"
#include "triangle.h"
#include "obj_load.h"
#include "texture.h"

using namespace std;

inline vec3 de_nan(const vec3 &c) {
    vec3 temp = c;
    if (!(temp[0] == temp[0])) temp[0] = 0;
    if (!(temp[1] == temp[1])) temp[1] = 0;
    if (!(temp[2] == temp[2])) temp[2] = 0;
    return temp;
}

vec3 color(const ray &r, hitable *world, pdf *light_pdf, int depth) {
    hit_record rec;
    if (world->hit(r, 0.001, FLT_MAX, rec)) {
        ray scattered;
        vec3 emitted = rec.mat_ptr->emitted(r, rec);
        scatter_record srec;
        float pdf_value;
        if (depth < 50 && rec.mat_ptr->scatter(r, rec, srec)) {
            if (light_pdf == nullptr) {
                scattered = ray(rec.p, srec.pdf_ptr->generate(rec), r.time());
                pdf_value = srec.pdf_ptr->value(rec, scattered.direction());
            } else {
                mixture_pdf p(light_pdf, srec.pdf_ptr);
                scattered = ray(rec.p, p.generate(rec), r.time());
                pdf_value = p.value(rec, scattered.direction());
            }

            return emitted +
                   srec.attenuation * rec.mat_ptr->scattering_pdf(r, rec, scattered) *
                   color(scattered, world, light_pdf, depth + 1) / pdf_value;
        } else {
            return emitted;
        }
    } else {
        return vec3(0, 0, 0);
    }
}

hitable *cornell_box() {
    hitable **list = new hitable *[8];
    int i = 0;
    pdf *cosine = new cosine_pdf();
    material *red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)), cosine);
    material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)), cosine);
    material *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)), cosine);
    material *light = new diffuse_light(new constant_texture(vec3(15, 15, 15)));
    list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
    list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
    list[i++] = new xz_rect(213, 343, 227, 332, 554, light);
    list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
    list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
    list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));
    list[i++] = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 165, 165), white), -18), vec3(130, 0, 65));
    list[i++] = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), white), 15), vec3(265, 0, 295));

    return new hitable_list(list, i);
}

void room(int nx, int ny, camera **cam, hitable **world, pdf **light) {
    vec3 lookfrom(0, 0, 4);
    vec3 lookat(0, 0, 0);
    vec3 up(0, 1, 0);
    float dist_to_focus = 10;
    float aperture = 0.0;
    float vfov = 50.0;
    *cam = new camera(lookfrom, lookat, up, vfov, float(nx) / float(ny), aperture, dist_to_focus, 0.0, 1.0);

    vector<hitable *> mesh;
    load_obj("room.obj", mesh);
    *world = new bvh(mesh.data(), mesh.size(), 0, FLT_MAX);
    hitable *light_shape = new sphere(vec3(0, 1.589, -1.274), 0.2, nullptr);
    *light = new hitable_pdf(light_shape);
}

void cup(int nx, int ny, camera **cam, hitable **world, pdf **light) {
    vec3 lookfrom(0.0, 0.64, 0.52);
    vec3 lookat(0.0, 0.40, 0.3);
    vec3 up(0, 1, 0);
    float dist_to_focus = 1;
    float aperture = 0.0;
    float vfov = 60.0;
    *cam = new camera(lookfrom, lookat, up, vfov, float(nx) / float(ny), aperture, dist_to_focus, 0.0, 1.0);

    hitable *light_shape = new yz_rect(1.0246, 2.0246, -0.5, 0.5, -2.058771896,
                                       new diffuse_light(new constant_texture(vec3(15, 15, 15))));
    *light = new hitable_pdf(light_shape);

    vector<hitable *> mesh;
    load_obj("cup.obj", mesh);
    mesh.emplace_back(light_shape);
    *world = new bvh(mesh.data(), mesh.size(), 0, FLT_MAX);
}

int main() {
    ofstream outfile("hellograph.ppm");
    int nx = 200, ny = 200, ns = 10;
    outfile << "P3\n" << nx << " " << ny << "\n255\n";

    camera *cam;
    hitable *world;
    pdf *light_pdf;
//    room(nx, ny, &cam, &world, &light_pdf);
    cup(nx, ny, &cam, &world, &light_pdf);

    for (int j = ny - 1; j >= 0; --j) {
        cout << j << endl;
        for (int i = 0; i < nx; ++i) {
            vec3 col(0, 0, 0);
            for (int s = 0; s < ns; ++s) {
                float u = float(i + drand48()) / nx;
                float v = float(j + drand48()) / ny;
                ray r = cam->get_ray(u, v);
                col += de_nan(color(r, world, light_pdf, 0));
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