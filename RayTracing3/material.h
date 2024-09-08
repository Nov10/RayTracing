#ifndef MATERIAL_H
#define MATERIAL_H

#include "helper.h"
#include "color.h"
#include "texture.h"
#include "onb.h"
#include "pdf.h"

class hit_record;
class scatter_record {
public:
    color attenuation;
    shared_ptr<pdf> pdf_ptr;
    bool skip_pdf;
    ray skip_pdf_ray;
};
//메터리얼(재질) 클래스
class material {
public:
    virtual ~material() = default;
    //빛 퍼짐(scatter)을 구현하는 추상 메소드
    virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered, const vector3& view, double& pdf) const {
        return false;
    }
    virtual bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const {
        return false;
    }
    virtual color emitted(
        const ray& r_in, const hit_record& rec, double u, double v, const point3& p
    ) const {
        return color(0, 0, 0);
    }
    virtual double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered)
        const {
        return 1;
    }
};

//램버시안 재질
class lambertian : public material {
public:
    lambertian(const color& albedo) : tex(make_shared<solid_color>(albedo)) {}
    lambertian(shared_ptr<texture> tex) : tex(tex) {}
    bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const override {
        srec.attenuation = tex->value(rec.u, rec.v, rec.point);
        srec.pdf_ptr = make_shared<cosine_pdf>(rec.normal);
        srec.skip_pdf = false;
        return true;
    }
bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered, const vector3& view, double& pdf) const override {
    //램버시안 반사는 관찰자가 보는 방향에 무관하게 빛을 반사시키므로
    //법선에 임의의 정규화된 벡터를 더하여 새로운 반사된 방향(scatter_direction)을 계산합니다.
    //auto scatter_direction = rec.normal + random_normalized_vector();
    //의도치 않게 영벡터가 되는 경우를 방지
    //if (scatter_direction.near_zero())
    //    scatter_direction = rec.normal;
    //auto d = dot(rec.normal, -r_in.direction());
    //scattered = ray(rec.point, scatter_direction, r_in.time());

    onb uvw;
    uvw.build_from_w(rec.normal);
    auto scatter_direction = uvw.local(random_cosine_direction());

    scattered = ray(rec.point, normalized(scatter_direction), r_in.time());
    attenuation = tex->value(rec.u, rec.v, rec.point);
    pdf = dot(uvw.w(), scattered.direction()) / pi;


    return true;
}
    double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const {
        auto cos_theta = dot(rec.normal, normalized(scattered.direction()));
        return cos_theta < 0 ? 0 : cos_theta / pi;
    }
private:
    color albedo;
    shared_ptr<texture> tex;
};
class extendedlambertian : public material {
public:
    extendedlambertian(const color& albedo, const double r) : tex(make_shared<solid_color>(albedo)), albedo(albedo), roughness(r){}
    extendedlambertian(shared_ptr<texture> tex) : tex(tex) {}

    double TrowbridgeRetizGGX(double a, const vector3 normal, const vector3 half) const
    {
        double mdotn = dot(half, normal);
        double mdotn2 = mdotn * mdotn;
        double a2 = a * a;
        double den = mdotn2 * a2 + (1 - mdotn2);
        double D = a2 / (pi * den * den);
        return D;
    }
    double SmithsSchlickGGX_Full(vector3 half, vector3 normal, vector3 w_o, vector3 w_i, double a) const
    {
        return 0;
        //double ndotv = dot(normal, half);
        //return ndotv / (ndotv * (1 - a) + a);
        //return SmithsSchlickGGX(half, normal, w_o, a) * SmithsSchlickGGX(half, normal, w_i, a);
    }
double SmithsSchlickGGX(vector3 normal, vector3 v, double a) const
{
    double ndotV = dot(normal, v);
    double k = (a +1)* (a + 1)/8;
    return ndotV / (ndotV * (1 - k) + k);
}

bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const override {
    double a = roughness; //거칠기
    vector3 view(-200, 0, 600);
    vector3 half = normalized(view + r_in.direction());
    double D = TrowbridgeRetizGGX(a, rec.normal, half);
    double G = SmithsSchlickGGX(half, -r_in.direction(), a) * SmithsSchlickGGX(half, vector3(-200, 0, 600), a);


    vector3 lambert = albedo / 3.1415926535;// *d;
    auto d = dot(rec.normal, -r_in.direction());


    double k_d = 0.9;

    srec.attenuation = (k_d * D * G * vector3(1, 1, 1) * albedo * d / 4) + ((1 - k_d) * lambert);

    srec.attenuation = tex->value(rec.u, rec.v, rec.point);
    srec.pdf_ptr = make_shared<cosine_pdf>(rec.normal);
    srec.skip_pdf = false;
    return true;
}
bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered, const vector3& view, double& pdf) const override {
    //램버시안 반사는 관찰자가 보는 방향에 무관하게 빛을 반사시키므로
    //법선에 임의의 정규화된 벡터를 더하여 새로운 반사된 방향(scatter_direction)을 계산합니다.
    auto scatter_direction = rec.normal+random_normalized_vector();

    //의도치 않게 영벡터가 되는 경우를 방지
    if (scatter_direction.near_zero())
        scatter_direction = rec.normal;

    //반사된 빛의 방향을 설정
    scattered = ray(rec.point, scatter_direction, r_in.time());

    double a = roughness; //거칠기
    vector3 half = normalized((view + r_in.direction()));
    double D = TrowbridgeRetizGGX(a, rec.normal, half);
    double G = SmithsSchlickGGX(half, -r_in.direction(),  a) * SmithsSchlickGGX(half, view, a);


    vector3 lambert = albedo / 3.1415926535;// *d;
    auto d = dot(rec.normal, -r_in.direction());


    double k_d = 1;

    attenuation = (k_d * D * G * vector3(1, 1, 1) * albedo * d/4) + ((1- k_d)*lambert);
    return true;
}
    double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const {
        auto cos_theta = dot(rec.normal, normalized(scattered.direction()));
        return cos_theta < 0 ? 0 : cos_theta / pi;
    }
private:
    double roughness;
    color albedo;
    shared_ptr<texture> tex;
};

//금속 재질
class metal : public material {
public:
    metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

    double TrowbridgeRetizGGX(double a, const vector3 normal, const vector3 half) const
    {
        double mdotn = dot(half, normal);
        double mdotn2 = mdotn * mdotn;
        double a2 = a * a;
        double den = mdotn2 * a2 + (1 - mdotn2);
        double D = a2 / (pi * den * den);
        return D;
    }
    bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const override {
        vector3 reflected = reflect(r_in.direction(), rec.normal);
        reflected = normalized(reflected) + (fuzz * random_normalized_vector());
        double a = 0.9;
        vector3 view(-200, 0, 600);
        vector3 half = normalized((view + r_in.direction()));
        double D = TrowbridgeRetizGGX(a, rec.normal, half);
        //double nd1 = dot(rec.normal, scattered.direction());
        auto d = dot(rec.normal, -r_in.direction());

        srec.attenuation = (albedo)*D * d / 2;
        srec.pdf_ptr = nullptr;
        srec.skip_pdf = true;
        srec.skip_pdf_ray = ray(rec.point, reflected, r_in.time());

        return true;
    }
bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered, const vector3& view, double& pdf) const override {
    //금속의 반사는 일반적인 반사 법칙을 따릅니다. 사전에 정의한 reflect 함수를 이용하여 반사된 광선의 방향을 계산합니다.
    vector3 reflected = reflect(normalized(r_in.direction()), rec.normal);
    //약간의 fuzzy를 추가하여 반사된 빛의 방향을 설정
    reflected = normalized(reflected) + (fuzz * random_normalized_vector());
    scattered = ray(rec.point, reflected, r_in.time());

    vector3 half = normalized((view + r_in.direction()));
    double a = 0.9;
    double D = TrowbridgeRetizGGX(a, rec.normal, half);
    //double nd1 = dot(rec.normal, scattered.direction());
    auto d = dot(rec.normal, -r_in.direction());

    attenuation = (albedo) * D * d/2;
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
    bool X(double x) const {
        return x > 0;
    }
    double TrowbridgeRetizGGX(double a, const vector3 normal, const vector3 half) const
    {
        double mdotn = dot(half, normal);
        //if (X(mdotn) == false)
        //    return 0;
        double mdotn2 = mdotn * mdotn;
        double a2 = a * a;
        double den = mdotn2 * a2 + (1 - mdotn2);
        double D = a / (pi * den * den);
        return D;
    }
bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered, const vector3& view, double& pdf) const override {
    attenuation = color(1.0, 1.0, 1.0);
    //공기와 재질의 굴절률의 비
    double refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

    //입사 광선의 코사인값과 사인값 계산
    vector3 unit_direction = normalized(r_in.direction());
    double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
    double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

    vector3 half = normalized((view + r_in.direction()));
    double cos_t2 = dot(half, normalized(-r_in.direction()));
    vector3 F0 = vector3(1, 1, 1);// *abs((refraction_ratio - 1) / (1 + refraction_ratio));
    //double D = TrowbridgeRetizGGX(a, rec.normal, half);
    vector3 F = reflectance_GGX(cos_t2, F0 * F0);

    auto d = dot(rec.normal, -r_in.direction());
    color albedo(1, 1, 1);

    //스넬의 법칙에 따라 굴절할 수 있는지 없는지를 계산
    bool cannot_refract = refraction_ratio * sin_theta > 1.0;
    vector3 direction;

    if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
        //굴절할 수 없다면, 반사
        direction = reflect(unit_direction, rec.normal);
    else
        //굴절할 수 있다면, 굴절
        direction = refract(unit_direction, rec.normal, refraction_ratio);
    double nd1 = dot(rec.normal, direction);
    attenuation = F/ nd1 * vector3(1, 1, 1) * d / 4;// / (nd1);// / (nd1);
    //반사/굴절된 빛의 방향을 설정
    scattered = ray(rec.point, direction, r_in.time());
    return true;
}
bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const override {
    vector3 view(0, 0, 1);
    vector3 half = normalized((view + r_in.direction()));
    double cos_t2 = dot(half, normalized(r_in.direction()));
    vector3 F0 = vector3(1, 1, 1);// *abs((ir - 1) / (1 + ir));
    vector3 F = reflectance_GGX(cos_t2, F0 * F0);

    srec.pdf_ptr = nullptr;
    srec.skip_pdf = true;
    double ri = rec.front_face ? (1.0 / ir) : ir;

    vector3 unit_direction = normalized(r_in.direction());
    double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
    double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

    bool cannot_refract = ri * sin_theta > 1.0;
    vector3 direction;

    if (cannot_refract || reflectance(cos_theta, ri) > random_double())
        direction = reflect(unit_direction, rec.normal);
    else
        direction = refract(unit_direction, rec.normal, ri);
    auto d = dot(rec.normal, -r_in.direction());
    double nd1 = dot(rec.normal, direction);
    srec.attenuation = color(1.0, 1.0, 1.0) * F * nd1;
    srec.skip_pdf_ray = ray(rec.point, direction, r_in.time());
    return true;
}
private:
double ir; //굴절률
vector3 reflectance_GGX(double cosine, vector3 f02) const {
    //Schlick's approximation의 근사를 이용
    //auto r0 = (1 - ref_idx) / (1 + ref_idx);
    //f02 = f02 * f02;
    return f02 + (vector3(1,1,1) - f02) * pow((1 - cosine), 5);
}
static double reflectance(double cosine, double ref_idx) {
    //Schlick's approximation의 근사를 이용
    auto r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
}
};
class diffuse_light : public material {
public:
    diffuse_light(shared_ptr<texture> tex) : tex(tex) {}
    diffuse_light(const color& emit) : tex(make_shared<solid_color>(emit)) {}

    color emitted(const ray& r_in, const hit_record& rec, double u, double v, const point3& p)
        const override {
        if (!rec.front_face)
            return color(0, 0, 0);
        return tex->value(u, v, p);
    }

private:
    shared_ptr<texture> tex;
};
class subsurface_scattering : public material {
public:
    subsurface_scattering(const color& albedo, double scattering_distance, double absorption_distance)
        : albedo(albedo),
        scattering_coefficient(1.0 / scattering_distance),
        absorption_coefficient(-log(albedo.length()) / absorption_distance) {}

    bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const override {
        // 산란 거리 샘플링: 확률적으로 산란 이벤트의 거리를 결정
        double distance = -log(random_double()) / scattering_coefficient;

        
        // 산란 거리가 표면까지의 거리보다 짧으면 산란 이벤트 발생
        if (distance < rec.t) {
            vector3 scatter_point = rec.point;

            // 무작위 산란 방향 (등방성 산란)
            vector3 scatter_direction = random_in_unit_sphere() * distance;

            srec.attenuation = exp(-absorption_coefficient * distance) * albedo;  // 흡수와 산란에 따른 감쇠
            srec.skip_pdf_ray = ray(scatter_point, scatter_direction, r_in.time());
            srec.skip_pdf = true;  // PDF를 사용하지 않고 직접 처리
            return true;

        }

        // 산란이 일어나지 않으면 표면에서 흡수만 발생
        return false;
    }

    double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const override {
        // 등방성 산란의 PDF는 1 / (4 * PI)
        return 1.0 / (4.0 * pi);
    }

    color emitted(const ray& r_in, const hit_record& rec, double u, double v, const point3& p) const override {
        return color(0, 0, 0);  // SSS에서는 자체 발광이 없으므로 0 반환
    }

private:
    color albedo;
    double scattering_coefficient;  // 산란 계수
    double absorption_coefficient;  // 흡수 계수
};

class isotropic : public material {
public:
    isotropic(const color& albedo) : tex(make_shared<solid_color>(albedo)) {}
    isotropic(shared_ptr<texture> tex) : tex(tex) {}

    bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const override {
        srec.attenuation = tex->value(rec.u, rec.v, rec.point);
        srec.pdf_ptr = make_shared<sphere_pdf>();
        srec.skip_pdf = false;
        return true;
    }
    double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered)
        const override {
        return 1 / (4 * pi);
    }
private:
    shared_ptr<texture> tex;
};
#endif