#ifndef HITTABLE_H
#define HITTABLE_H

#include "rtweekend.h"

class material;

//Hit에 대한 정보를 담는 클래스
class hit_record {
public:
    point3 point;
    vector3 normal;
    shared_ptr<material> mat;
    double t;

    bool front_face;

    void set_face_normal(const ray& r, const vector3& outward_normal) {
        //내적이 양수이면 두 벡터는 같은 방향을, 음수이면 다른 방향을 보고 있음을 뜻합니다.
        //광선과 법선의 내적을 계산하여 두 벡터가 같은 방향/다른 방향을 보고 있는지 계산합니다.
        //만일 광선과 법선이 같은 방향이라면, 광선은 물체의 표면 안에 있다는 것을 뜻합니다.
        front_face = dot(r.direction(), outward_normal) < 0;
        //광선이 물체 내부에 있다면, 법선의 방향을 뒤집습니다.
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable {
public:
    virtual ~hittable() = default;

    virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;
};

#endif