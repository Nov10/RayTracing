#ifndef HITTABLE_H
#define HITTABLE_H

#include "helper.h"

class material;

//Hit에 대한 정보를 담는 클래스
class hit_record {
public:
    point3 point; //충돌 지점
    vector3 normal; //충돌 표면의 법선
    shared_ptr<material> mat; //충돌 물체의 재질
    double t; //광선의 매개변수

    bool front_face; //광선과 "물체의 바깥을 향하는 법선"이 같은 방향을 보고 있는지 여부

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

    //물체와 광선의 충돌을 계산하여 충돌 여부를 반환(충돌할 경우, rec에 정보가 들어감)
    virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;
};

#endif