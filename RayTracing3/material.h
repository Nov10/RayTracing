#ifndef MATERIAL_H
#define MATERIAL_H

#include "helper.h"
#include "color.h"

class hit_record;

//메터리얼(재질) 클래스
class material {
public:
    virtual ~material() = default;
    //빛 퍼짐(scatter)을 구현하는 추상 메소드
    virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const = 0;
};

//램버시안 재질
class lambertian : public material {
public:
    lambertian(const color& a) : albedo(a) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
        //램버시안 반사는 관찰자가 보는 방향에 무관하게 빛을 반사시키므로
        //법선에 임의의 정규화된 벡터를 더하여 새로운 반사된 방향(scatter_direction)을 계산합니다.
        auto scatter_direction = rec.normal + random_normalized_vector();

        //의도치 않게 영벡터가 되는 경우를 방지
        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;

        //반사된 빛의 방향을 설정
        scattered = ray(rec.point, scatter_direction);
        attenuation = albedo;
        return true;
    }

private:
    color albedo;
};

//금속 재질
class metal : public material {
public:
    metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
        //금속의 반사는 일반적인 반사 법칙을 따릅니다. 사전에 정의한 reflect 함수를 이용하여 반사된 광선의 방향을 계산합니다.
        vector3 reflected = reflect(normalized(r_in.direction()), rec.normal);
        //약간의 fuzzy를 추가하여 반사된 빛의 방향을 설정
        scattered = ray(rec.point, reflected + fuzz * random_normalized_vector());
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }

private:
    color albedo;
    double fuzz;
};

//유전체 재질
class dielectric : public material {
public:
    dielectric(double index_of_refraction) : ir(index_of_refraction) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
        attenuation = color(1.0, 1.0, 1.0);
        //공기와 재질의 굴절률의 비
        double refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

        //입사 광선의 코사인값과 사인값 계산
        vector3 unit_direction = normalized(r_in.direction());
        double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

        //스넬의 법칙에 따라 굴절할 수 있는지 없는지를 계산
        bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        vector3 direction;

        if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
            //굴절할 수 없다면, 반사
            direction = reflect(unit_direction, rec.normal);
        else
            //굴절할 수 있다면, 굴절
            direction = refract(unit_direction, rec.normal, refraction_ratio);

        //반사/굴절된 빛의 방향을 설정
        scattered = ray(rec.point, direction);
        return true;
    }

private:
    double ir; //굴절률

    static double reflectance(double cosine, double ref_idx) {
        //Schlick's approximation의 근사를 이용
        auto r0 = (1 - ref_idx) / (1 + ref_idx);
        r0 = r0 * r0;
        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }
};

#endif