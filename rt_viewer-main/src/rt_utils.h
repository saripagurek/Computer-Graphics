#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/constants.hpp>
#include <glm/gtx/quaternion.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <cstdint>
#include <algorithm>
#include <map>

// Utility Functions

inline double degrees_to_radians(double degrees) {
    return degrees * 3.14f / 180.0;
}

inline double random_double() {
    // Returns a random real in [0,1).
    return rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max-min)*random_double();
}

float length_squared(glm::vec3 e) {
    return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
}

float length(glm::vec3 e) {
    return sqrt(length_squared(e));
}
/*
static glm::vec3 random() {
    return glm::vec3(random_double(), random_double(), random_double());
}
*/
static glm::vec3 random(double min, double max) {
    return glm::vec3(random_double(min,max), random_double(min,max), random_double(min,max));
}

inline glm::vec3 unit_vector(glm::vec3 v) {
    return v / length(v);
}

inline glm::vec3 random_in_unit_sphere() {
    while (true) {
        auto p = random(-1,1);
        if (length_squared(p) < 1)
            return p;
    }
}

inline glm::vec3 random_unit_vector() {
    return unit_vector(random_in_unit_sphere());
}

inline glm::vec3 random_on_hemisphere(const glm::vec3& normal) {
    glm::vec3 on_unit_sphere = random_unit_vector();
    if (glm::dot(on_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
        return on_unit_sphere;
    else
        return -on_unit_sphere;
}

glm::vec3 reflect(const glm::vec3& v, const glm::vec3& n) {
    return v - 2 * glm::dot(v, n) * n;
}