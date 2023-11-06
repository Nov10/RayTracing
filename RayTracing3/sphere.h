#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"

//��(�浹 ������ ��ü)
class sphere : public hittable {
public:
    sphere(point3 _center, double _radius, shared_ptr<material> _material) : center(_center), radius(_radius), mat(_material) {}

    //��ü�� ��ü�� �浹�� ���
    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        //������ ���� �������� ������ ��, t�� ���� �������������� ��Ÿ������ ��
        //t�� ���� ������������ ������� �����
        vector3 oc = r.origin() - center;
        auto a = r.direction().length_squared();
        auto half_b = dot(oc, r.direction());
        auto c = oc.length_squared() - radius * radius;

        //�Ǻ����� ���� ���� ���缺�� �Ǵ�
        auto discriminant = half_b * half_b - a * c;
        //�Ǻ����� 0���� �۴ٸ�, �ذ� �������� ����
        if (discriminant < 0) return false;
        auto sqrtd = sqrt(discriminant);

        //������������ �� �� �� ������ �´� ���� ����
        auto root = (-half_b - sqrtd) / a;
        if (!ray_t.surrounds(root)) {
            root = (-half_b + sqrtd) / a;
            if (!ray_t.surrounds(root))
                return false;
        }
        
        rec.t = root; //���� ���������� �Ű������� ��
        rec.point = r.at(rec.t); //���� �̿��Ͽ� ������ ���� ������ ���
        vector3 outward_normal = (rec.point - center) / radius; //������ ���
        rec.set_face_normal(r, outward_normal); //������ ������ ����Ͽ� ������ ������ ����
        rec.mat = mat; //���� ����

        return true;
    }

private:
    point3 center; //���� �߽�
    double radius; //���� ������
    shared_ptr<material> mat; //���� ����
};

#endif