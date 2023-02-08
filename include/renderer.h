#ifndef _RENDERER_H
#define _RENDERER_H

#include <memory>
#include <cmath>
#include "basic.h"

class Material {
public:
    static constexpr float n_air = 1.0f;

public:
    enum class Type {
        ROUGH,
        REFLECTIVE,
        REFRACTIVE
    } type;

    Vector3D color;
    int shininess;   // convergence index of reflected lights [1, 128]
    float metalness; // index of metalness
    Vector3D F0;     // basic reflectivity
    float n;         // index of refraction

    Vector3D Ka; // Ambient coefficient
    Vector3D Kd; // Diffuse coefficient
    Vector3D Ks; // Specular coefficient

    Material(Type t) : type(t) {}
    Material(const Material &) = default;
    Material &operator=(const Material &) = default;

    // rough
    Material(const Vector3D &c, float d, float s, int shine, float metal)
        : type(Type::ROUGH), color(c), shininess(shine), metalness(metal), Ka(0.5 * color), Kd(d * color), Ks(s * color) {}

    // reflective
    Material(const Vector3D &c, float d, float s, int shine, float metal, const Vector3D &F0)
        : type(Type::REFLECTIVE), color(c), shininess(shine), metalness(metal), F0(Vector3D::mix(F0, color, metalness)), Ka(0.5 * color), Kd(d * color), Ks(s * color) {}

    // refractive
    Material(const Vector3D &c, float d, float s, int shine, float metal, const Vector3D &F0, float n)
        : type(Type::REFRACTIVE), color(c), shininess(shine), metalness(metal), n(n), F0(Vector3D::mix(F0, color, metalness)), Ka(0.5 * color), Kd(d * color), Ks(s * color) {}
};

class Texture {
};

class Renderer {
public:
    std::shared_ptr<Material> material;
    std::shared_ptr<Texture> texture;
};

#endif // _RENDERER_H