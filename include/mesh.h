#ifndef _MESH_H
#define _MESH_H

#include "basic.h"
#include "obj.h"

class Mesh {
public:
    virtual Hit intersection(const Ray &) = 0;
};

class Sphere : public Mesh {
public:
    Point center;
    float radius;

    Sphere(const Point &c, float r) : center(c), radius(r) {}
    Sphere(const Sphere &) = default;
    Sphere &operator=(const Sphere &) = default;

    Hit intersection(const Ray &ray) override;
};

class Plane : public Mesh {
public:
    Point lb; // left bottom point
    Vector3D right;
    Vector3D up;

    Plane(const Point &lb, const Vector3D &r, const Vector3D &u) : lb(lb), right(r), up(u) {}
    Plane(const Plane &) = default;
    Plane &operator=(const Plane &) = default;

    Hit intersection(const Ray &ray) override;
};

// other polygon mesh model
class Model : public Mesh {
public:
    std::vector<Face> face;

    Model() = default;
    Model(const OBJ &obj) : face(obj.face) {}

    Hit intersection(const Ray &ray) override;
};

#endif // _MESH_H