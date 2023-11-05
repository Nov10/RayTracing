#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class ray {
public:
    ray() {}

    ray(const point3& origin, const vector3& direction) : orig(origin), dir(direction) {}

    point3 origin() const { return orig; }
    vector3 direction() const { return dir; }

    //�Ű������� �̿��Ͽ� ���� ���� �� ���� ��ġ�� ���
    point3 at(double t) const {
        return orig + t * dir;
    }

private:
    point3 orig; //������
    vector3 dir; //����
};

#endif
