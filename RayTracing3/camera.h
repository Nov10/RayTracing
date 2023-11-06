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
    int    image_width = 100; //렌더링 될 이미지의 가로 너비
    int    samples_per_pixel = 10;   // 픽셀당 샘플링 횟수
    int    max_depth = 10;   // 광선이 튕기는 횟수의 최댓값
    double vfov = 90;  // 세로의 FOV 각도
    point3 lookfrom = point3(0, 0, -1);  // 카메라의 위치
    point3 lookat = point3(0, 0, 0);   // 카메라의 방향
    vector3   vup = vector3(0, 1, 0);     // 카메라의 좌표계를 기준으로 하는 윗방향

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

        //색상을 샘플 횟수로 나눔
        auto scale = 1.0 / samples_per_pixel;
        r *= scale;
        g *= scale;
        b *= scale;

        //선형 공간에서 감마 공간으로 변환
        r = linear_to_gamma(r);
        g = linear_to_gamma(g);
        b = linear_to_gamma(b);

        //0~1에서 0~255로 변환
        static const interval intensity(0.000, 0.999);
        return color(256 * intensity.clamp(r), 256 * intensity.clamp(g), 256 * intensity.clamp(b));
    }

private:
    int    image_height; //렌더링 이미지의 높이. 가로 너비에 종속됨
    point3 center;         //카메라의 중심 위치
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
        //(시야각의 절반)에 대한 tan값
        //tan(theta / 2) = 0.5height / focus_dist
        auto h = tan(theta / 2);
        //height = 2 * focus_dist * tan(theta/2)
        auto viewport_height = 2 * h * focus_dist;
        //viewport_height : viewport_width = image_height : image_width
        //viewport_width = viewport_height * image_width / image_height
        auto viewport_width = viewport_height * (static_cast<double>(image_width) / image_height);

        //카메라와 관련된 기저 벡터를 계산      
        w = normalized(lookfrom - lookat); //카메라가 보는 방향의 반대 방향
        u = normalized(cross(vup, w)); //카메라의 오른쪽 방향
        v = cross(w, u); //카메라의 위쪽 방향
        
        //렌더링 이미지의 벡터를 계산
        //렌더링 이미지의 왼쪽 -> 오른쪽 벡터
        vector3 viewport_u = viewport_width * u;
        //렌더링 이미지의 위쪽 -> 아래쪽 벡터
        vector3 viewport_v = viewport_height * -v;

        //펙살당 벡터를 계산
        //한 픽셀당, 왼쪽 -> 오른쪽 벡터
        pixel_delta_u = viewport_u / image_width;
        //한 픽셀당, 위쪽 -> 오른쪽 벡터
        pixel_delta_v = viewport_v / image_height;

        // Calculate the location of the upper left pixel.
        //가장 왼쪽 위의 위치를 계산
        //왼쪽 위의 위치 = (중심) + (앞으로 떨어진 거리) + (왼쪽) + (오른쪽)
        //왼쪽 위의 위치 = center + (-focus_dist * w) + (-viewport_u/2) + (viewport_v/2)
        auto viewport_upper_left = center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
        //가장 왼쪽 위 픽셀의 위치는 픽셀당 벡터의 절반만큼만 이동한 위치
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        auto defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

    //(i, j) 픽셀에 대해 사용될 광선을 생성하여 반환
    ray get_ray(int i, int j) const {
        //렌더링할 픽셀의 중심 위치
        auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
        //렌더링할 픽셀 내에서 랜덤한 지점을 선택
        auto pixel_sample = pixel_center + pixel_sample_square();

        //광선의 시작점, defocus 효과를 적용하기 위한 랜덤한 위치를 선택
        auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
        //광선의 방향
        auto ray_direction = pixel_sample - ray_origin;

        return ray(ray_origin, ray_direction);
    }

    //defocus 효과를 위한 임의의 지점을 반환(변위)
    point3 defocus_disk_sample() const {
        //defocus disk 내부에서 임의의 한 점을 선택
        auto p = random_in_unit_disk();
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }

    //한 픽셀 내부에서 임의의 지점을 반환(변위)
    vector3 pixel_sample_square() const {
        auto px = -0.5 + random_double();
        auto py = -0.5 + random_double();
        return (px * pixel_delta_u) + (py * pixel_delta_v);
    }

    //특정 광선이 발사되었을 때, 해당 광선에서 나오는 색상을 반환
    color ray_color(const ray& r, int depth, const hittable& world) const {
        hit_record rec;

        //광선이 최대로 튕기는 횟수를 제한
        if (depth <= 0)
            return color(0, 0, 0);

        if (world.hit(r, interval(0.001, infinity), rec)) {
            ray scattered;
            color attenuation;

            //빛에 새롭게 감지된 물체에서 퍼질 경우
            if (rec.mat->scatter(r, rec, attenuation, scattered))
                //현재 감지된 물체에서 나온 색과 새롭게 감지된 물체에서 나온 색을 곱함
                //재귀적으로 작동하므로, 위에서 설정한 최대까지 빛이 튕기는 것을 구현할 수 있습니다.
                return attenuation * ray_color(scattered, depth - 1, world);

            //빛이 퍼지지 않을 경우, 검은색을 반환
            return color(0, 0, 0);
        }

        //배경 그라데이션(하늘)
        vector3 unit_direction = normalized(r.direction());
        auto a = 0.5 * (unit_direction.y() + 1.0);
        return ((1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0));
    }
};

#endif