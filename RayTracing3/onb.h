#ifndef ONB_H
#define ONB_H

#include "helper.h"

class onb {
public:
    onb() {}

    vector3 operator[](int i) const { return axis[i]; }
    vector3& operator[](int i) { return axis[i]; }

    vector3 u() const { return axis[0]; }
    vector3 v() const { return axis[1]; }
    vector3 w() const { return axis[2]; }

    vector3 local(double a, double b, double c) const {
        return a * u() + b * v() + c * w();
    }

    vector3 local(const vector3& a) const {
        return a.x() * u() + a.y() * v() + a.z() * w();
    }

    void build_from_w(const vector3& w) {
        vector3 unit_w = normalized(w);
        vector3 a = (fabs(unit_w.x()) > 0.9) ? vector3(0, 1, 0) : vector3(1, 0, 0);
        vector3 v = normalized(cross(unit_w, a));
        vector3 u = cross(unit_w, v);
        axis[0] = u;
        axis[1] = v;
        axis[2] = unit_w;
    }

public:
    vector3 axis[3];
};


#endif