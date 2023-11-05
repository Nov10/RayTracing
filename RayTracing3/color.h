#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"
#include <iostream>

using color = vector3;

inline double linear_to_gamma(double linear_component)
{
	return sqrt(linear_component);
}

#endif