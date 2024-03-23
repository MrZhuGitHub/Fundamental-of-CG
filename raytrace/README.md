# description
build and run in linux x86-64

implement raytrace by native cpp code, no dependent other lib

including below feature:
- supported to  basic material, dielectric, metal, lambertian.
- supported to multi-threading and bounding volume hierarchy to improve rendering speed.
- supported to monte carlo integration based on rendering equation.
- supported to importance sample for light.
- supported to add texture for object.

# build

cmake ..
make -j8


# run

./raytrace -example [param1] -samples [param2]

param1:

1 - RayTracingInOneWeekend

2 - rayTracingTheNextWeek

3 - rayTracingTheRestOfYourLife

0 - renderEarth

param2: generate samples ray in each pixel


# result

