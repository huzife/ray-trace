#ifndef _OBJECTS_H
#define _OBJECTS_H

#include <unordered_set>
#include <functional>
#include <memory>
#include <numeric>
#include <thread>
#include <atomic>
#include "basic.h"
#include "renderer.h"
#include "mesh.h"

// object
class Object {
public:
    std::shared_ptr<Mesh> mesh_filter;
    Renderer mesh_renderer;
};

using HitInfo = std::pair<Hit, std::shared_ptr<Object>>;

class Scene;

// light
class Light {
public:
    std::shared_ptr<Scene> parent_scene;

public:
    Vector3D intensity;

    Light(const Vector3D &i) : intensity(i) {}

    void setParentScene(std::shared_ptr<Scene> scene);

    virtual Vector3D getColor(const Hit &hit, std::shared_ptr<Object> hit_object, const Vector3D &V) = 0;
};

class AmbientLight : public Light {
public:
    AmbientLight(const Vector3D &i) : Light(i) {}

    Vector3D getColor(const Hit &hit, std::shared_ptr<Object> hit_object, const Vector3D &V) override;
};

class PointLight : public Light {
public:
    Point position;

    PointLight(const Vector3D &i, const Point &p) : Light(i), position(p) {}

    Vector3D getColor(const Hit &hit, std::shared_ptr<Object> hit_object, const Vector3D &V) override;
};

// camera
class Camera {
public:
    Point eye;      // camera position
    Point center;   // center of sight
    Vector3D up;    // directly above the camera
    Vector3D right; // directly to the right of the camere
    float fovy;     // fov in y axis
    float width;    // window width(real size, not pixel size)
    float height;   // window height(real size, not pixel size)

    bool checkUpAndRight();

    void setCamera(const Point &e, const Point &c, const Vector3D &up, float fovy);

    void setPerspective(int windowWidth, int windowHeight);

    Ray getRay(int x, int y, int windowWidth, int windowHeight);
};

// scene
class Scene : public std::enable_shared_from_this<Scene> {
public:
    static constexpr int maxdepth = 5;

public:
    std::unordered_set<std::shared_ptr<Object>> objects;
    std::unordered_set<std::shared_ptr<Light>> lights;
    std::shared_ptr<AmbientLight> ambient_light;
    std::shared_ptr<Camera> camera;

    Vector3D background;

    void addObject(std::shared_ptr<Object> object);

    void delObject(std::shared_ptr<Object> object);

    void addLight(std::shared_ptr<Light> light);

    void delLight(std::shared_ptr<Light> light);

    HitInfo getIntersection(Ray &ray);

    bool underShadow(Ray &ray, float t_max);

    Vector3D rayTrace(Ray &ray, int depth);

    void render(unsigned char *pixel, int windowWidth, int windowHeight);
};

#endif // _OBJECTS_H