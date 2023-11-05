#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class ray {
public:
    ray() {}

    ray(const point3& origin, const vector3& direction) : orig(origin), dir(direction) {}

    point3 origin() const { return orig; }
    vector3 direction() const { return dir; }

    //매개변수를 이용하여 광선 위의 한 점의 위치를 계산
    point3 at(double t) const {
        return orig + t * dir;
    }

private:
    point3 orig; //시작점
    vector3 dir; //방향
};

#endif
