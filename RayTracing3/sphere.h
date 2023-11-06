#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"

//구(충돌 가능한 물체)
class sphere : public hittable {
public:
    sphere(point3 _center, double _radius, shared_ptr<material> _material) : center(_center), radius(_radius), mat(_material) {}

    //구체와 물체의 충돌을 계산
    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        //광선과 구의 방정식을 연립한 후, t에 대한 이차방정식으로 나타내었을 때
        //t에 대한 이차방정식의 계수들을 계산함
        vector3 oc = r.origin() - center;
        auto a = r.direction().length_squared();
        auto half_b = dot(oc, r.direction());
        auto c = oc.length_squared() - radius * radius;

        //판별식을 통해 해의 존재성을 판단
        auto discriminant = half_b * half_b - a * c;
        //판별식이 0보다 작다면, 해가 존재하지 않음
        if (discriminant < 0) return false;
        auto sqrtd = sqrt(discriminant);

        //이차방정식의 두 근 중 범위에 맞는 근을 선택
        auto root = (-half_b - sqrtd) / a;
        if (!ray_t.surrounds(root)) {
            root = (-half_b + sqrtd) / a;
            if (!ray_t.surrounds(root))
                return false;
        }
        
        rec.t = root; //근이 직선에서의 매개변수가 됨
        rec.point = r.at(rec.t); //근을 이용하여 직선과 구의 교점을 계산
        vector3 outward_normal = (rec.point - center) / radius; //법선을 계산
        rec.set_face_normal(r, outward_normal); //광선의 방향을 고려하여 법선의 방향을 수정
        rec.mat = mat; //재질 설정

        return true;
    }

private:
    point3 center; //구의 중심
    double radius; //구의 반지름
    shared_ptr<material> mat; //구의 재질
};

#endif