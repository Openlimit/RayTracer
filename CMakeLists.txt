cmake_minimum_required(VERSION 3.10)
project(RayTracer)

set(CMAKE_CXX_STANDARD 11)
#aux_source_directory(. DIR_SRCS)

add_executable(RayTracer main.cpp vec3.h ray.h hitable.h sphere.h hitable_list.h camera.h material.h lambertian.h metal.h dielectric.h drand48.h)