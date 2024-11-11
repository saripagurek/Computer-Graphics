#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/component_wise.hpp>
#include "rt_utils.h"

namespace rt {

class Material;

struct HitRecord {
    float t;
    glm::vec3 p;
    glm::vec3 normal;
    std::shared_ptr<Material> mat;
};

class Material {
  public:
    Material(glm::vec3 c) : color(c) {}
    glm::vec3 color;
    virtual ~Material() = default;

    virtual bool scatter(
        const Ray& r_in, const HitRecord& rec, Ray& scattered) const = 0;
};

class Metal : public Material {
  public:
    Metal(const glm::vec3& a) : Material(a) {}

    virtual bool scatter(const Ray& r_in, const HitRecord& rec, Ray& scattered)
    const override {
        glm::vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        scattered = Ray(rec.p, reflected);
        return true;
    }
};

class Lambertian : public Material {
  public:
    Lambertian(const glm::vec3& a) : Material(a) {}

    virtual bool scatter(const Ray& r_in, const HitRecord& rec, Ray& scattered)
    const override {
        //auto scatter_direction = rec.normal + random_unit_vector();
        auto scatter_direction = random_on_hemisphere(rec.normal);
        scattered = Ray(rec.p, scatter_direction);
        return true;
    }

};
/*
class FlatColour : public Material {
    public:
        glm::vec3 color;
        glm::vec3 scattered;

        FlatColour() {}
        FlatColour(glm::vec3 c) {
            color = c;
        }

};*/

class Hitable {
public:
    virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const = 0;
};

} // namespace rt
