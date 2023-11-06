#ifndef CAMERA_H
#define CAMERA_H
#include <opencv2/opencv.hpp>

#include "helper.h"

#include "color.h"
#include "hittable.h"
#include "material.h"

class camera {
public:
    double aspect_ratio = 1.0;  // Ratio of image width over height
    int    image_width = 100; //������ �� �̹����� ���� �ʺ�
    int    samples_per_pixel = 10;   // �ȼ��� ���ø� Ƚ��
    int    max_depth = 10;   // ������ ƨ��� Ƚ���� �ִ�
    double vfov = 90;  // ������ FOV ����
    point3 lookfrom = point3(0, 0, -1);  // ī�޶��� ��ġ
    point3 lookat = point3(0, 0, 0);   // ī�޶��� ����
    vector3   vup = vector3(0, 1, 0);     // ī�޶��� ��ǥ�踦 �������� �ϴ� ������

    double defocus_angle = 0;  // Variation angle of rays through each pixel
    double focus_dist = 10;    // Distance from camera lookfrom point to plane of perfect focus

    void render(const hittable& world) {
        initialize();

        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        cv::Mat image(image_height, image_width, CV_8UC3);

        for (int j = 0; j < image_height; ++j) {
            std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
            for (int i = 0; i < image_width; ++i) {
                auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
                auto ray_direction = pixel_center - center;
                ray r(center, ray_direction);

                color pixel_color(0, 0, 0);
                for (int sample = 0; sample < samples_per_pixel; ++sample) {
                    ray r = get_ray(i, j);
                    pixel_color += ray_color(r, max_depth, world);
                }
                pixel_color = write_color(pixel_color, samples_per_pixel);

                image.at<cv::Vec3b>(j, i) = cv::Vec3b(pixel_color.z(), pixel_color.y(), pixel_color.x());
            }
        }

        // Show the image
        cv::imshow("Image", image);
        cv::waitKey(0); // Wait for a key event

        cv::imwrite("picture.ppm", image);

        std::clog << "\rDone.                 \n";

    }

    color write_color(color pixel_color, int samples_per_pixel) {
        auto r = pixel_color.x();
        auto g = pixel_color.y();
        auto b = pixel_color.z();

        //������ ���� Ƚ���� ����
        auto scale = 1.0 / samples_per_pixel;
        r *= scale;
        g *= scale;
        b *= scale;

        //���� �������� ���� �������� ��ȯ
        r = linear_to_gamma(r);
        g = linear_to_gamma(g);
        b = linear_to_gamma(b);

        //0~1���� 0~255�� ��ȯ
        static const interval intensity(0.000, 0.999);
        return color(256 * intensity.clamp(r), 256 * intensity.clamp(g), 256 * intensity.clamp(b));
    }

private:
    int    image_height; //������ �̹����� ����. ���� �ʺ� ���ӵ�
    point3 center;         //ī�޶��� �߽� ��ġ
    point3 pixel00_loc;    // Location of pixel 0, 0
    vector3   pixel_delta_u;  // Offset to pixel to the right
    vector3   pixel_delta_v;  // Offset to pixel below
    vector3   u, v, w;        // Camera frame basis vectors
    vector3   defocus_disk_u;  // Defocus disk horizontal radius
    vector3   defocus_disk_v;  // Defocus disk vertical radius

    void initialize() {
        image_height = static_cast<int>(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        center = lookfrom;

        auto theta = degrees_to_radians(vfov);
        //(�þ߰��� ����)�� ���� tan��
        //tan(theta / 2) = 0.5height / focus_dist
        auto h = tan(theta / 2);
        //height = 2 * focus_dist * tan(theta/2)
        auto viewport_height = 2 * h * focus_dist;
        //viewport_height : viewport_width = image_height : image_width
        //viewport_width = viewport_height * image_width / image_height
        auto viewport_width = viewport_height * (static_cast<double>(image_width) / image_height);

        //ī�޶�� ���õ� ���� ���͸� ���      
        w = normalized(lookfrom - lookat); //ī�޶� ���� ������ �ݴ� ����
        u = normalized(cross(vup, w)); //ī�޶��� ������ ����
        v = cross(w, u); //ī�޶��� ���� ����
        
        //������ �̹����� ���͸� ���
        //������ �̹����� ���� -> ������ ����
        vector3 viewport_u = viewport_width * u;
        //������ �̹����� ���� -> �Ʒ��� ����
        vector3 viewport_v = viewport_height * -v;

        //���� ���͸� ���
        //�� �ȼ���, ���� -> ������ ����
        pixel_delta_u = viewport_u / image_width;
        //�� �ȼ���, ���� -> ������ ����
        pixel_delta_v = viewport_v / image_height;

        // Calculate the location of the upper left pixel.
        //���� ���� ���� ��ġ�� ���
        //���� ���� ��ġ = (�߽�) + (������ ������ �Ÿ�) + (����) + (������)
        //���� ���� ��ġ = center + (-focus_dist * w) + (-viewport_u/2) + (viewport_v/2)
        auto viewport_upper_left = center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
        //���� ���� �� �ȼ��� ��ġ�� �ȼ��� ������ ���ݸ�ŭ�� �̵��� ��ġ
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        auto defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

    //(i, j) �ȼ��� ���� ���� ������ �����Ͽ� ��ȯ
    ray get_ray(int i, int j) const {
        //�������� �ȼ��� �߽� ��ġ
        auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
        //�������� �ȼ� ������ ������ ������ ����
        auto pixel_sample = pixel_center + pixel_sample_square();

        //������ ������, defocus ȿ���� �����ϱ� ���� ������ ��ġ�� ����
        auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
        //������ ����
        auto ray_direction = pixel_sample - ray_origin;

        return ray(ray_origin, ray_direction);
    }

    //defocus ȿ���� ���� ������ ������ ��ȯ(����)
    point3 defocus_disk_sample() const {
        //defocus disk ���ο��� ������ �� ���� ����
        auto p = random_in_unit_disk();
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }

    //�� �ȼ� ���ο��� ������ ������ ��ȯ(����)
    vector3 pixel_sample_square() const {
        auto px = -0.5 + random_double();
        auto py = -0.5 + random_double();
        return (px * pixel_delta_u) + (py * pixel_delta_v);
    }

    //Ư�� ������ �߻�Ǿ��� ��, �ش� �������� ������ ������ ��ȯ
    color ray_color(const ray& r, int depth, const hittable& world) const {
        hit_record rec;

        //������ �ִ�� ƨ��� Ƚ���� ����
        if (depth <= 0)
            return color(0, 0, 0);

        if (world.hit(r, interval(0.001, infinity), rec)) {
            ray scattered;
            color attenuation;

            //���� ���Ӱ� ������ ��ü���� ���� ���
            if (rec.mat->scatter(r, rec, attenuation, scattered))
                //���� ������ ��ü���� ���� ���� ���Ӱ� ������ ��ü���� ���� ���� ����
                //��������� �۵��ϹǷ�, ������ ������ �ִ���� ���� ƨ��� ���� ������ �� �ֽ��ϴ�.
                return attenuation * ray_color(scattered, depth - 1, world);

            //���� ������ ���� ���, �������� ��ȯ
            return color(0, 0, 0);
        }

        //��� �׶��̼�(�ϴ�)
        vector3 unit_direction = normalized(r.direction());
        auto a = 0.5 * (unit_direction.y() + 1.0);
        return ((1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0));
    }
};

#endif