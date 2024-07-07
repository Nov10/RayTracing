#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"
#include "aabb.h"

//구(충돌 가능한 물체)
class sphere : public hittable {
public:
    // Stationary Sphere
    sphere(const point3& center, double radius, shared_ptr<material> mat)
        : center1(center), radius(fmax(0, radius)), mat(mat), is_moving(false)
    {
        auto rvec = vector3(radius, radius, radius);
        bbox = aabb(center1 - rvec, center1 + rvec);
    }

    // Moving Sphere
    sphere(const point3& center1, const point3& center2, double radius,
        shared_ptr<material> mat)
        : center1(center1), radius(fmax(0, radius)), mat(mat), is_moving(true)
    {
        auto rvec = vector3(radius, radius, radius);
        aabb box1(center1 - rvec, center1 + rvec);
        aabb box2(center2 - rvec, center2 + rvec);
        bbox = aabb(box1, box2);

        center_vec = center2 - center1;
    }

    //구체와 물체의 충돌을 계산
    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        //광선과 구의 방정식을 연립한 후, t에 대한 이차방정식으로 나타내었을 때
        //t에 대한 이차방정식의 계수들을 계산함
        point3 center = is_moving ? sphere_center(r.time()) : center1;
        //vector3 oc = r.origin() - center;
        vector3 oc =r.origin() - center;
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
        get_sphere_uv(outward_normal, rec.u, rec.v);
        rec.mat = mat; //재질 설정

        return true;
    }
    aabb bounding_box() const override { return bbox; }

    double pdf_value(const point3& origin, const vector3& direction) const override {
        // This method only works for stationary spheres.

        hit_record rec;
        if (!this->hit(ray(origin, direction), interval(0.001, infinity), rec))
            return 0;

        auto cos_theta_max = sqrt(1 - radius * radius / (center1 - origin).length_squared());
        auto solid_angle = 2 * pi * (1 - cos_theta_max);

        return  1 / solid_angle;
    }

    vector3 random(const point3& origin) const override {
        vector3 direction = center1 - origin;
        auto distance_squared = direction.length_squared();
        onb uvw;
        uvw.build_from_w(direction);
        return uvw.local(random_to_sphere(radius, distance_squared));
    }

private:
    point3 center1; //구의 중심
    double radius; //구의 반지름
    shared_ptr<material> mat; //구의 재질
    bool is_moving;
    vector3 center_vec;
    aabb bbox;
    point3 sphere_center(double time) const {
        // Linearly interpolate from center1 to center2 according to time, where t=0 yields
        // center1, and t=1 yields center2.
        return center1 + time * center_vec;
    }
    static void get_sphere_uv(const point3& p, double& u, double& v) {
        // p: a given point on the sphere of radius one, centered at the origin.
        // u: returned value [0,1] of angle around the Y axis from X=-1.
        // v: returned value [0,1] of angle from Y=-1 to Y=+1.
        //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
        //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
        //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

        auto theta = acos(-p.y());
        auto phi = atan2(-p.z(), p.x()) + pi;

        u = phi / (2 * pi);
        v = theta / pi;
    }
    static vector3 random_to_sphere(double radius, double distance_squared) {
        auto r1 = random_double();
        auto r2 = random_double();
        auto z = 1 + r2 * (sqrt(1 - radius * radius / distance_squared) - 1);

        auto phi = 2 * pi * r1;
        auto x = cos(phi) * sqrt(1 - z * z);
        auto y = sin(phi) * sqrt(1 - z * z);

        return vector3(x, y, z);
    }
};

#endif