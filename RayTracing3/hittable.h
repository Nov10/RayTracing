#ifndef HITTABLE_H
#define HITTABLE_H

#include "helper.h"

class material;

//Hit�� ���� ������ ��� Ŭ����
class hit_record {
public:
    point3 point; //�浹 ����
    vector3 normal; //�浹 ǥ���� ����
    shared_ptr<material> mat; //�浹 ��ü�� ����
    double t; //������ �Ű�����

    bool front_face; //������ "��ü�� �ٱ��� ���ϴ� ����"�� ���� ������ ���� �ִ��� ����

    void set_face_normal(const ray& r, const vector3& outward_normal) {
        //������ ����̸� �� ���ʹ� ���� ������, �����̸� �ٸ� ������ ���� ������ ���մϴ�.
        //������ ������ ������ ����Ͽ� �� ���Ͱ� ���� ����/�ٸ� ������ ���� �ִ��� ����մϴ�.
        //���� ������ ������ ���� �����̶��, ������ ��ü�� ǥ�� �ȿ� �ִٴ� ���� ���մϴ�.
        front_face = dot(r.direction(), outward_normal) < 0;
        //������ ��ü ���ο� �ִٸ�, ������ ������ �������ϴ�.
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable {
public:
    virtual ~hittable() = default;

    //��ü�� ������ �浹�� ����Ͽ� �浹 ���θ� ��ȯ(�浹�� ���, rec�� ������ ��)
    virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;
};

#endif