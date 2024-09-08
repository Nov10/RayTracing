#include <opencv2/opencv.hpp>
#include "helper.h"

#include "bvh.h"
#include "camera.h"
#include "hittable_list.h"
#include "sphere.h"
#include "color.h"
#include "material.h"
#include "texture.h"
#include "quad.h"
#include "constant_medium.h"

color ray_color(const ray& r, const hittable& world) {
    hit_record rec;
    if (world.hit(r, interval(0, infinity), rec)) {
        return 0.5 * (rec.normal + color(1, 1, 1)) * 255;
    }

    vector3 unit_direction = normalized(r.direction());
    auto a = 0.5 * (unit_direction.y() + 1.0);
    return ((1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0)) * 255;
}
//void checkered_spheres() {
//    hittable_list world;
//    hittable_list lights;
//
//    auto checker = make_shared<checker_texture>(0.32, color(.2, .3, .1), color(.9, .9, .9));
//
//    world.add(make_shared<sphere>(point3(0, -10, 0), 10, make_shared<lambertian>(checker)));
//    world.add(make_shared<sphere>(point3(0, 10, 0), 10, make_shared<lambertian>(checker)));
//
//    camera cam;
//
//    cam.aspect_ratio = 16.0 / 9.0;
//    cam.image_width = 400;
//    cam.samples_per_pixel = 100;
//    cam.max_depth = 50;
//    cam.background = color(0.70, 0.80, 1.00);
//
//    cam.vfov = 20;
//    cam.lookfrom = point3(13, 2, 3);
//    cam.lookat = point3(0, 0, 0);
//    cam.vup = vector3(0, 1, 0);
//
//    cam.defocus_angle = 0;
//
//    cam.render(world);
//}
//void bouncing_spheres() {
//    hittable_list world;
//    hittable_list lights;
//
//    auto checker = make_shared<checker_texture>(0.32, color(.1, .1, .2), color(.9, .9, .9));
//    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(checker)));
//
//
//    //auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
//    //world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));
//
//    for (int a = -2; a <2; a++) {
//        for (int b = -2; b < 2; b++) {
//            auto choose_mat = random_double();
//            point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());
//
//            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
//                shared_ptr<material> sphere_material;
//
//                //if (choose_mat < 0.8) {
//                //    // diffuse
//                //    //auto albedo = color::random(0.5, 1);
//                //    auto albedo = color::random(0.1, 1);// *color::random();
//                //    //auto fuzz = random_double(0, 0.05);
//                //    auto rough = random_double(0, 0.8);
//                //    sphere_material = make_shared<extendedlambertian>(albedo, rough);
//                //    //sphere_material = make_shared<metal>(albedo, fuzz);
//                //    //sphere_material = make_shared<extendedlambertian>(albedo);
//                //    world.add(make_shared<sphere>(center, 0.2, sphere_material));
//                //    //auto center2 = center + vector3(0, random_double(0, .5), 0);
//                //    //world.add(make_shared<sphere>(center, center2, 0.2, sphere_material));
//                //}
//                //else if (choose_mat < 0.95) {
//                //    //metal
//                //    auto albedo = color::random(0.5, 1);
//                //    auto fuzz = random_double(0, 0.5);
//                //    sphere_material = make_shared<metal>(albedo, fuzz);
//                //    world.add(make_shared<sphere>(center, 0.2, sphere_material));
//                //}
//            //    else 
//            {
//                    auto albedo = color::random() * color::random();
//                    //here_material = make_shared<dielectric>(8);
//                    sphere_material = make_shared<extendedlambertian>(vector3(1,1,1), 0.01);
//                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
//                    // glass
//                    //sphere_material = make_shared<dielectric>(1.5);
//                    //world.add(make_shared<sphere>(center, 0.2, sphere_material));
//                }
//            }
//        }
//    }
//
//    //auto material1 = make_shared<dielectric>(1.3);
//    //world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));
//
//    //auto material2 = make_shared<extendedlambertian>(color(0.4, 0.2, 0.1), 0.5);
//    //world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));
//
//    //auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
//    //world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));
//
//    world = hittable_list(make_shared<bvh_node>(world));
//
//    camera cam;
//
//    cam.aspect_ratio = 16.0 / 9.0;
//    //cam.aspect_ratio = 1.0;
//    cam.image_width = 720;
//    cam.samples_per_pixel = 30;
//    cam.max_depth = 50;
//
//    cam.background = color(0.70, 0.80, 1.00);
//    cam.vfov = 20;
//    cam.lookfrom = point3(13, 2, 3);
//    cam.lookat = point3(0, 0, 0);
//    cam.vup = vector3(0, 1, 0);
//
//    cam.defocus_angle = 0.0;
//    cam.focus_dist = 10.0;
//
//    cam.render(world);
//}
//void earth() {
//    hittable_list lights;
//    auto earth_texture = make_shared<image_texture>("earthmap.jpg");
//    auto earth_surface = make_shared<lambertian>(earth_texture);
//    auto globe = make_shared<sphere>(point3(0, 0, 0), 2, earth_surface);
//
//    camera cam;
//
//    cam.aspect_ratio = 16.0 / 9.0;
//    cam.image_width = 720;
//    cam.samples_per_pixel = 10;
//    cam.max_depth = 50;
//
//    cam.background = color(0.70, 0.80, 1.00);
//    cam.vfov = 20;
//    cam.lookfrom = point3(0, 0, 12);
//    cam.lookat = point3(0, 0, 0);
//    cam.vup = vector3(0, 1, 0);
//
//    cam.defocus_angle = 0;
//
//    cam.render(hittable_list(globe));
//}
//void perlin_spheres() {
//    hittable_list world;
//
//    auto pertext = make_shared<noise_texture>(8);
//    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
//    world.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));
//
//    camera cam;
//
//    cam.aspect_ratio = 16.0 / 9.0;
//    cam.image_width = 400;
//    cam.samples_per_pixel = 10;
//    cam.max_depth = 20;
//
//    cam.background = color(0.70, 0.80, 1.00);
//    cam.vfov = 20;
//    cam.lookfrom = point3(13, 2, -5);
//    cam.lookat = point3(0, 0, 0);
//    cam.vup = vector3(0, 1, 0);
//
//    cam.defocus_angle = 0;
//
//    cam.render(world);
//}
void quads() {
    hittable_list world;
    hittable_list lights;

    // Materials
    auto left_red = make_shared<lambertian>(color(1.0, 0.2, 0.2));
    auto back_green = make_shared<lambertian>(color(0.2, 1.0, 0.2));
    auto right_blue = make_shared<lambertian>(color(0.2, 0.2, 1.0));
    auto upper_orange = make_shared<lambertian>(color(1.0, 0.5, 0.0));
    auto lower_teal = make_shared<lambertian>(color(0.2, 0.8, 0.8));

    // Quads
    world.add(make_shared<quad>(point3(-3, -2, 5), vector3(0, 0, -4), vector3(0, 4, 0), left_red));
    world.add(make_shared<quad>(point3(-2, -2, 0), vector3(4, 0, 0), vector3(0, 4, 0), back_green));
    world.add(make_shared<quad>(point3(3, -2, 1), vector3(0, 0, 4), vector3(0, 4, 0), right_blue));
    world.add(make_shared<quad>(point3(-2, 3, 1), vector3(4, 0, 0), vector3(0, 0, 4), upper_orange));
    world.add(make_shared<quad>(point3(-2, -3, 5), vector3(4, 0, 0), vector3(0, 0, -4), lower_teal));

    camera cam;

    cam.aspect_ratio = 1.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 3;
    cam.max_depth = 10;

    cam.background = color(0.70, 0.80, 1.00);
    cam.vfov = 80;
    cam.lookfrom = point3(0, 0, 9);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vector3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render(world, lights);
}
void simple_light() {
    hittable_list world;
    hittable_list lights;

    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(vector3(0.7,0.5,1)*0.4)));
    //world.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<extendedlambertian>(vector3(0.1, 0.2, 1), 0.1)));
    //world.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<metal>(vector3(1,0.2,0.15), 0.03)));
    world.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<extendedlambertian>(vector3(1, 0.2, 0.15), 0.1)));

    auto difflight = make_shared<diffuse_light>(color(8, 8, 8)*3);
    world.add(make_shared<sphere>(point3(5, 10, 0), 2, difflight));
    lights.add(make_shared<sphere>(point3(5, 10, 0), 2, difflight));
    //world.add(make_shared<quad>(point3(3, 1, -2), vector3(2, 0, 0), vector3(0, 0, 2), difflight));
    //lights.add(make_shared<quad>(point3(3, 1, -2), vector3(2, 0, 0), vector3(0, 0, 2), difflight));

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 500;
    cam.samples_per_pixel = 300;
    cam.max_depth = 50;
    cam.background = color(0.01, 0.05, 0.1);

    cam.vfov = 20;
    cam.lookfrom = point3(26, 3, 6);
    cam.lookat = point3(0, 2, 0);
    cam.vup = vector3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render(world, lights);
}
void cornell_box() {
    hittable_list world;

    auto red = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .15, .85));
    auto green3 = make_shared<lambertian>(color(.12, .75, .15));
    auto light = make_shared<diffuse_light>(color(15, 15, 15) * 3);

    auto sss = make_shared< subsurface_scattering>(color(.12, .15, .85), 0.4, 0.6);

    world.add(make_shared<quad>(point3(555, 0, 0), vector3(0, 555, 0), vector3(0, 0, 555), green));
    world.add(make_shared<quad>(point3(0, 0, 0), vector3(0, 555, 0), vector3(0, 0, 555), red));
    world.add(make_shared<quad>(point3(343, 554, 332), vector3(-130, 0, 0), vector3(0, 0, -105), light));
    world.add(make_shared<quad>(point3(0, 0, 0), vector3(555, 0, 0), vector3(0, 0, 555), white));
    world.add(make_shared<quad>(point3(555, 555, 555), vector3(-555, 0, 0), vector3(0, 0, -555), green3));
    world.add(make_shared<quad>(point3(0, 0, 555), vector3(555, 0, 0), vector3(0, 555, 0), white));
    
    // Light
    world.add(make_shared<quad>(point3(213, 554, 227), vector3(130, 0, 0), vector3(0, 0, 105), light));
    // Light Sources
    hittable_list lights;
    auto m = shared_ptr<material>();
    lights.add(make_shared<quad>(point3(343, 554, 332), vector3(-130, 0, 0), vector3(0, 0, -105), m));
    lights.add(make_shared<sphere>(point3(190, 90, 190), 90, m));

    auto glass2 = make_shared<metal>(color(0.15, 0.1, 0.86), 0.001);
    // Box
    
    shared_ptr<hittable> box1 = box(point3(0, 0, 0), point3(165, 330, 165), sss);
    box1 = make_shared<rotate_y>(box1, 30);
    box1 = make_shared<translate>(box1, vector3(265, 0, 295));
    world.add(box1);

    // Glass Sphere
    auto glass = make_shared<dielectric>(0.32);
    world.add(make_shared<sphere>(point3(190, 90, 190), 90, glass));

    // Glass Sphere
    auto met = make_shared<metal>(color(0.75, 0.2, 0.76), 0.01);
    world.add(make_shared<sphere>(point3(340, 50, 80), 50, met));
    
    camera cam;

    cam.aspect_ratio = 1.0;
    cam.image_width = 750;
    cam.samples_per_pixel = 12*12;
    cam.max_depth = 20;
    cam.background = color(1,1,1);

    cam.vfov = 40;
    cam.lookfrom = point3(278, 278, -800);
    cam.lookat = point3(278, 278, 0);
    cam.vup = vector3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render(world, lights);

}
//void cornell_smoke() {
//    hittable_list world;
//
//    auto red = make_shared<lambertian>(color(.65, .05, .05));
//    auto white = make_shared<lambertian>(color(.73, .73, .73));
//    auto green = make_shared<lambertian>(color(.12, .45, .15));
//    auto light = make_shared<diffuse_light>(color(7, 7, 7));
//
//    world.add(make_shared<quad>(point3(555, 0, 0), vector3(0, 555, 0), vector3(0, 0, 555), green));
//    world.add(make_shared<quad>(point3(0, 0, 0), vector3(0, 555, 0), vector3(0, 0, 555), red));
//    world.add(make_shared<quad>(point3(113, 554, 127), vector3(330, 0, 0), vector3(0, 0, 305), light));
//    world.add(make_shared<quad>(point3(0, 555, 0), vector3(555, 0, 0), vector3(0, 0, 555), white));
//    world.add(make_shared<quad>(point3(0, 0, 0), vector3(555, 0, 0), vector3(0, 0, 555), white));
//    world.add(make_shared<quad>(point3(0, 0, 555), vector3(555, 0, 0), vector3(0, 555, 0), white));
//
//    shared_ptr<hittable> box1 = box(point3(0, 0, 0), point3(165, 330, 165), white);
//    box1 = make_shared<rotate_y>(box1, 15);
//    box1 = make_shared<translate>(box1, vector3(265, 0, 295));
//
//    shared_ptr<hittable> box2 = box(point3(0, 0, 0), point3(165, 165, 165), white);
//    box2 = make_shared<rotate_y>(box2, -18);
//    box2 = make_shared<translate>(box2, vector3(130, 0, 65));
//
//    world.add(make_shared<constant_medium>(box1, 0.01, color(0, 0, 0)));
//    world.add(make_shared<constant_medium>(box2, 0.01, color(1, 1, 1)));
//
//    camera cam;
//
//    cam.aspect_ratio = 1.0;
//    cam.image_width = 600;
//    cam.samples_per_pixel = 4;
//    cam.max_depth = 10;
//    cam.background = color(0, 0, 0);
//
//    cam.vfov = 40;
//    cam.lookfrom = point3(278, 278, -800);
//    cam.lookat = point3(278, 278, 0);
//    cam.vup = vector3(0, 1, 0);
//
//    cam.defocus_angle = 0;
//
//    cam.render(world);
//}
void final_scene(int image_width, int samples_per_pixel, int max_depth) {
    hittable_list boxes1;
    hittable_list lights;
    //auto ground = make_shared<lambertian>(color(0.48, 0.83, 0.53));

    int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i * w;
            auto z0 = -1000.0 + j * w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = random_double(1, 101);
            auto z1 = z0 + w;
            auto rough = random_double(0, 0.9);
            auto ground = make_shared<extendedlambertian>(color(0.48, 0.83, 0.53), rough);
            boxes1.add(box(point3(x0, y0, z0), point3(x1, y1, z1), ground));
        }
    }

    hittable_list world;

    world.add(make_shared<bvh_node>(boxes1));

    auto light = make_shared<diffuse_light>(color(7.5, 7.5, 7.5) * 0.7);
    world.add(make_shared<quad>(point3(123, 554, 147), vector3(300, 0, 0), vector3(0, 0, 265), light));
    lights.add(make_shared<quad>(point3(123, 554, 147), vector3(300, 0, 0), vector3(0, 0, 265), light));

    auto center1 = point3(400, 400, 200);
    auto sphere_material = make_shared<extendedlambertian>(color(0.7, 0.3, 0.1), 0.3);
    world.add(make_shared<sphere>(center1, 50, sphere_material));

    world.add(make_shared<sphere>(point3(260, 150, 45), 50, make_shared<dielectric>(1.4)));
    world.add(make_shared<sphere>(
        point3(0, 150, 145), 50, make_shared<metal>(color(0.8, 0.8, 0.9), 0.9)
    ));

    auto boundary = make_shared<sphere>(point3(380, 150, 145), 70, make_shared<dielectric>(1.2));
    world.add(boundary);
    world.add(make_shared<constant_medium>(boundary, 0.2, color(0.2, 0.4, 0.9)));
    boundary = make_shared<sphere>(point3(0, 0, 0), 5000, make_shared<dielectric>(1.5));
    world.add(make_shared<constant_medium>(boundary, .0001, color(1, 1, 1)));

    //auto emat = make_shared<lambertian>(make_shared<image_texture>("earthmap.jpg"));
    auto emat = make_shared<metal>(color(0.9, 0.9, 0.95), 0.1);
    world.add(box(point3(300, 100, 300), point3(400, 450, 400), emat));
    auto pertext = make_shared<noise_texture>(0.1);
    world.add(make_shared<rotate_y>(make_shared<sphere>(point3(220, 280, 300), 80, make_shared<lambertian>(pertext)), -10));

    hittable_list boxes2;
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        auto white = make_shared<extendedlambertian>(color(random_double(0, 0.95), random_double(0, 0.95), random_double(0, 0.95)), random_double(0, 0.95));
        boxes2.add(make_shared<sphere>(point3::random(0, 165), 10, white));
    }

    world.add(make_shared<translate>(
        make_shared<rotate_y>(
            make_shared<bvh_node>(boxes2), 15),
        vector3(-100, 270, 395)
    )
    );

    camera cam;

    cam.aspect_ratio = 1.0;
    cam.image_width = image_width;
    cam.samples_per_pixel = samples_per_pixel;
    cam.max_depth = max_depth;
    cam.background = color(0, 0, 0);

    cam.vfov = 40;
    cam.lookfrom = point3(478, 278, -600);
    cam.lookat = point3(278, 278, 0);
    cam.vup = vector3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render(world, lights);
}
//void tmp()
//{
//    hittable_list world;
//
//    auto red = make_shared<lambertian>(color(.65, .05, .05));
//    auto white = make_shared<lambertian>(color(.73, .73, .73));
//    auto green = make_shared<lambertian>(color(.12, .45, .15));
//    auto light = make_shared<diffuse_light>(color(15, 15, 15));
//
//    // Cornell box sides
//    world.add(make_shared<quad>(point3(555, 0, 0), vector3(0, 0, 555), vector3(0, 555, 0), green));
//    world.add(make_shared<quad>(point3(0, 0, 555), vector3(0, 0, -555), vector3(0, 555, 0), red));
//    world.add(make_shared<quad>(point3(0, 555, 0), vector3(555, 0, 0), vector3(0, 0, 555), white));
//    world.add(make_shared<quad>(point3(0, 0, 555), vector3(555, 0, 0), vector3(0, 0, -555), white));
//    world.add(make_shared<quad>(point3(555, 0, 555), vector3(-555, 0, 0), vector3(0, 555, 0), white));
//
//    // Light
//    world.add(make_shared<quad>(point3(213, 554, 227), vector3(130, 0, 0), vector3(0, 0, 105), light));
//
//    // Box 1
//    shared_ptr<hittable> box1 = box(point3(0, 0, 0), point3(165, 330, 165), white);
//    box1 = make_shared<rotate_y>(box1, 15);
//    box1 = make_shared<translate>(box1, vector3(265, 0, 295));
//    world.add(box1);
//
//    // Box 2
//    shared_ptr<hittable> box2 = box(point3(0, 0, 0), point3(165, 165, 165), white);
//    box2 = make_shared<rotate_y>(box2, -18);
//    box2 = make_shared<translate>(box2, vector3(130, 0, 65));
//    world.add(box2);
//
//    camera cam;
//
//    cam.aspect_ratio = 1.0;
//    cam.image_width = 300;
//    cam.samples_per_pixel = 80;
//    cam.max_depth = 8;
//    cam.background = color(0, 0, 0);
//
//    cam.vfov = 40;
//    cam.lookfrom = point3(278, 278, -800);
//    cam.lookat = point3(278, 278, 0);
//    cam.vup = vector3(0, 1, 0);
//
//    cam.defocus_angle = 0;
//
//    cam.render(world);
//}
int main() {
    switch (7) {
    //case 1: bouncing_spheres();  break;
    //case 2: checkered_spheres(); break;
    //case 3:  earth();             break;
    //case 4:  perlin_spheres();     break;
    case 5:  quads();              break;
    case 6:  simple_light();       break;
    case 7:  cornell_box();        break;

    //case 8:  cornell_smoke();      break;
    //case 9:  final_scene(800, 10000, 40); break;
    //case 10: tmp(); break;
    default: final_scene(500, 150, 18); break;
    }
}