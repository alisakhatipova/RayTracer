#pragma once

#include "Types.h"
using namespace glm;
struct CScene
{
	float hole_mass;
	float hole_rad;
	float disk_coeff;
    float earth_rad = 1e+9;
    vec3 earth1_pos = vec3(2e+9, 2e+9, 112e+9);
    vec3 earth2_pos = vec3(0, -2e+9, 110e+9);
};
