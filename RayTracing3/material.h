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
//���͸���(����) Ŭ����
class material {
public:
    virtual ~material() = default;
    //�� ����(scatter)�� �����ϴ� �߻� �޼ҵ�
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

//�����þ� ����
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
    //�����þ� �ݻ�� �����ڰ� ���� ���⿡ �����ϰ� ���� �ݻ��Ű�Ƿ�
    //������ ������ ����ȭ�� ���͸� ���Ͽ� ���ο� �ݻ�� ����(scatter_direction)�� ����մϴ�.
    //auto scatter_direction = rec.normal + random_normalized_vector();
    //�ǵ�ġ �ʰ� �����Ͱ� �Ǵ� ��츦 ����
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
    double a = roughness; //��ĥ��
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
    //�����þ� �ݻ�� �����ڰ� ���� ���⿡ �����ϰ� ���� �ݻ��Ű�Ƿ�
    //������ ������ ����ȭ�� ���͸� ���Ͽ� ���ο� �ݻ�� ����(scatter_direction)�� ����մϴ�.
    auto scatter_direction = rec.normal+random_normalized_vector();

    //�ǵ�ġ �ʰ� �����Ͱ� �Ǵ� ��츦 ����
    if (scatter_direction.near_zero())
        scatter_direction = rec.normal;

    //�ݻ�� ���� ������ ����
    scattered = ray(rec.point, scatter_direction, r_in.time());

    double a = roughness; //��ĥ��
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

//�ݼ� ����
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
    //�ݼ��� �ݻ�� �Ϲ����� �ݻ� ��Ģ�� �����ϴ�. ������ ������ reflect �Լ��� �̿��Ͽ� �ݻ�� ������ ������ ����մϴ�.
    vector3 reflected = reflect(normalized(r_in.direction()), rec.normal);
    //�ణ�� fuzzy�� �߰��Ͽ� �ݻ�� ���� ������ ����
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

//����ü ����
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
    //����� ������ �������� ��
    double refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

    //�Ի� ������ �ڻ��ΰ��� ���ΰ� ���
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

    //������ ��Ģ�� ���� ������ �� �ִ��� �������� ���
    bool cannot_refract = refraction_ratio * sin_theta > 1.0;
    vector3 direction;

    if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
        //������ �� ���ٸ�, �ݻ�
        direction = reflect(unit_direction, rec.normal);
    else
        //������ �� �ִٸ�, ����
        direction = refract(unit_direction, rec.normal, refraction_ratio);
    double nd1 = dot(rec.normal, direction);
    attenuation = F/ nd1 * vector3(1, 1, 1) * d / 4;// / (nd1);// / (nd1);
    //�ݻ�/������ ���� ������ ����
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
double ir; //������
vector3 reflectance_GGX(double cosine, vector3 f02) const {
    //Schlick's approximation�� �ٻ縦 �̿�
    //auto r0 = (1 - ref_idx) / (1 + ref_idx);
    //f02 = f02 * f02;
    return f02 + (vector3(1,1,1) - f02) * pow((1 - cosine), 5);
}
static double reflectance(double cosine, double ref_idx) {
    //Schlick's approximation�� �ٻ縦 �̿�
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
        // ��� �Ÿ� ���ø�: Ȯ�������� ��� �̺�Ʈ�� �Ÿ��� ����
        double distance = -log(random_double()) / scattering_coefficient;

        
        // ��� �Ÿ��� ǥ������� �Ÿ����� ª���� ��� �̺�Ʈ �߻�
        if (distance < rec.t) {
            vector3 scatter_point = rec.point;

            // ������ ��� ���� (��漺 ���)
            vector3 scatter_direction = random_in_unit_sphere() * distance;

            srec.attenuation = exp(-absorption_coefficient * distance) * albedo;  // ����� ����� ���� ����
            srec.skip_pdf_ray = ray(scatter_point, scatter_direction, r_in.time());
            srec.skip_pdf = true;  // PDF�� ������� �ʰ� ���� ó��
            return true;

        }

        // ����� �Ͼ�� ������ ǥ�鿡�� ����� �߻�
        return false;
    }

    double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const override {
        // ��漺 ����� PDF�� 1 / (4 * PI)
        return 1.0 / (4.0 * pi);
    }

    color emitted(const ray& r_in, const hit_record& rec, double u, double v, const point3& p) const override {
        return color(0, 0, 0);  // SSS������ ��ü �߱��� �����Ƿ� 0 ��ȯ
    }

private:
    color albedo;
    double scattering_coefficient;  // ��� ���
    double absorption_coefficient;  // ��� ���
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