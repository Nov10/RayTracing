#ifndef MATERIAL_H
#define MATERIAL_H

#include "helper.h"
#include "color.h"

class hit_record;

//���͸���(����) Ŭ����
class material {
public:
    virtual ~material() = default;
    //�� ����(scatter)�� �����ϴ� �߻� �޼ҵ�
    virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const = 0;
};

//�����þ� ����
class lambertian : public material {
public:
    lambertian(const color& a) : albedo(a) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
        //�����þ� �ݻ�� �����ڰ� ���� ���⿡ �����ϰ� ���� �ݻ��Ű�Ƿ�
        //������ ������ ����ȭ�� ���͸� ���Ͽ� ���ο� �ݻ�� ����(scatter_direction)�� ����մϴ�.
        auto scatter_direction = rec.normal + random_normalized_vector();

        //�ǵ�ġ �ʰ� �����Ͱ� �Ǵ� ��츦 ����
        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;

        //�ݻ�� ���� ������ ����
        scattered = ray(rec.point, scatter_direction);
        attenuation = albedo;
        return true;
    }

private:
    color albedo;
};

//�ݼ� ����
class metal : public material {
public:
    metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
        //�ݼ��� �ݻ�� �Ϲ����� �ݻ� ��Ģ�� �����ϴ�. ������ ������ reflect �Լ��� �̿��Ͽ� �ݻ�� ������ ������ ����մϴ�.
        vector3 reflected = reflect(normalized(r_in.direction()), rec.normal);
        //�ణ�� fuzzy�� �߰��Ͽ� �ݻ�� ���� ������ ����
        scattered = ray(rec.point, reflected + fuzz * random_normalized_vector());
        attenuation = albedo;
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

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
        attenuation = color(1.0, 1.0, 1.0);
        //����� ������ �������� ��
        double refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

        //�Ի� ������ �ڻ��ΰ��� ���ΰ� ���
        vector3 unit_direction = normalized(r_in.direction());
        double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

        //������ ��Ģ�� ���� ������ �� �ִ��� �������� ���
        bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        vector3 direction;

        if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
            //������ �� ���ٸ�, �ݻ�
            direction = reflect(unit_direction, rec.normal);
        else
            //������ �� �ִٸ�, ����
            direction = refract(unit_direction, rec.normal, refraction_ratio);

        //�ݻ�/������ ���� ������ ����
        scattered = ray(rec.point, direction);
        return true;
    }

private:
    double ir; //������

    static double reflectance(double cosine, double ref_idx) {
        //Schlick's approximation�� �ٻ縦 �̿�
        auto r0 = (1 - ref_idx) / (1 + ref_idx);
        r0 = r0 * r0;
        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }
};

#endif