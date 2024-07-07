#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H
#include "aabb.h"
#include "hittable.h"

#include <memory>
#include <vector>

using std::shared_ptr;
using std::make_shared;

class hittable_list : public hittable {
public:
    std::vector<shared_ptr<hittable>> objects;

    hittable_list() {}
    hittable_list(shared_ptr<hittable> object) { add(object); }

    //모든 물체를 삭제
    void clear() { objects.clear(); }

    //물체를 추가
    void add(shared_ptr<hittable> object) {
        objects.push_back(object);
        bbox = aabb(bbox, object->bounding_box());
    }
    double pdf_value(const point3& origin, const vector3& direction) const override {
        auto weight = 1.0 / objects.size();
        auto sum = 0.0;

        for (const auto& object : objects)
            sum += weight * object->pdf_value(origin, direction);

        return sum;
    }

    vector3 random(const point3& origin) const override {
        auto int_size = int(objects.size());
        return objects[random_int(0, int_size - 1)]->random(origin);
    }
    //특정 광선과 목록에 있는 모든 물체의 충돌을 계산
    //(물체와 충돌할 경우, rec에 정보가 들어감)
    //(여러 물체와 충돌할 경우, 가장 가까운 충돌된 물체에 대한 정보가 담김)
    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        hit_record temp_rec;
        bool hit_anything = false;
        auto closest_so_far = ray_t.max;

        //존재하는 모든 물체에 대해 계산
        for (const auto& object : objects) {
            //간격을 설정하여 가장 가까운 물체만을 최종적으로 남김
            if (object->hit(r, interval(ray_t.min, closest_so_far), temp_rec)) {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }

   
    aabb bounding_box() const override { return bbox; }

private:
    aabb bbox;
};

#endif