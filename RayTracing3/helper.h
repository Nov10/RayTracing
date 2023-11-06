#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <limits>
#include <memory>
#include <cstdlib>

using std::shared_ptr;
using std::make_shared;
using std::sqrt;

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;


//육십분법에서 라디안으로 단위를 변환하여 반환
inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

//[0,1)에 속하는 랜덤한 실수를 반환
inline double random_double() {
    return rand() / (RAND_MAX + 1.0);
}

//[min,max)에 속하는 랜덤한 실수를 반환
inline double random_double(double min, double max) {
    return min + (max - min) * random_double();
}

#include "ray.h"
#include "vec3.h"
#include "interval.h"

#endif