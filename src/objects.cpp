#include "objects.h"

bool Camera::checkUpAndRight() {
    Vector3D n = eye - center; // opposite of view direction

    // check if n and up are collinear
    float angle = Vector3D::angle(up, n);
    if (fequal(angle, 0.0f) || fequal(angle, 180.0f)) return false;

    // calculate right
    right = Vector3D::cross(up, n).normalized();

    // make sure up and n are perpendicular
    if (!fequal(angle, 90.0f)) {
        up = Vector3D::cross(n, right);
    }

    // normalize up
    up.normalize();

    return true;
}

Vector3D AmbientLight::getColor(const std::shared_ptr<Scene> &scene, const Hit &hit, const Vector3D &V) {
    if (Vector3D::dot(V, std::get<Vector3D>(hit)) > 0) return Vector3D::zero;
    auto object = scene->hit_object;
    auto material = object->mesh_renderer.material;
    Vector3D ret = material->Ka * intensity;

    return ret;
}

Vector3D PointLight::getColor(const std::shared_ptr<Scene> &scene, const Hit &hit, const Vector3D &V) {
    auto object = scene->hit_object;
    auto material = object->mesh_renderer.material;

    // hit point and normal
    Point hit_point = std::get<Point>(hit);
    Vector3D hit_normal = std::get<Vector3D>(hit);

    if (Vector3D::dot(V, hit_normal) > 0) return Vector3D::zero;

    // direction from hit point to light
    Vector3D L = position - hit_point;
    float t_max = L.magnitude();
    L.normalize();

    // shadow check
    Ray detect_ray(hit_point, L);
    Hit detect_hit = scene->getIntersection(detect_ray, false);
    float t_hit = std::get<float>(detect_hit);
    if (!fequal(t_hit, -1) && t_hit < t_max) return Vector3D::zero;

    // calculate cosA, A is the angle of L and n
    float a = Vector3D::dot(L, hit_normal);
    if (a <= 0) return Vector3D::zero;

    // calculate cosB, B is the angle of H and n, H is the half-range vector of L and V
    Vector3D H = L - V;
    float b = Vector3D::dot(H, hit_normal) / H.magnitude();

    // diffuse and reflect
    Vector3D diffuse = material->Kd * intensity * a;
    Vector3D reflect = material->Ks * intensity * std::pow(b, material->shininess);

    return diffuse + reflect;
}

void Camera::setCamera(const Point &e, const Point &c, const Vector3D &up, float fovy) {
    this->eye = e;
    this->center = c;
    this->up = up;
    this->fovy = fovy;

    bool ret = checkUpAndRight();
    assert(ret);
}

void Camera::setPerspective(int windowWidth, int windowHeight) {
    height = Point::distance(eye, center) * std::tan(Angle::degToRad(fovy / 2)) * 2;
    width = height * windowWidth / windowHeight;
}

Ray Camera::getRay(int x, int y, int windowWidth, int windowHeight) {
    float w = width / windowWidth;
    float h = height / windowHeight;
    float dx = -(windowWidth - 1) * w / 2 + x * w;
    float dy = -(windowHeight - 1) * h / 2 + y * h;
    Vector3D dir = center - eye + dx * right + dy * up;
    return Ray(eye, dir);
}

// Scene
void Scene::addObject(std::shared_ptr<Object> object) {
    objects.insert(object);
}

void Scene::delObject(std::shared_ptr<Object> object) {
    objects.erase(object);
}

void Scene::addLight(std::shared_ptr<Light> light) {
    assert(std::dynamic_pointer_cast<AmbientLight>(light) == nullptr);
    lights.insert(light);
}

void Scene::delLight(std::shared_ptr<Light> light) {
    assert(std::dynamic_pointer_cast<AmbientLight>(light) == nullptr);
    lights.erase(light);
}

Hit Scene::getIntersection(Ray &ray, bool change_hit_object) {
    // calculate the nearest hit
    if (change_hit_object) hit_object = nullptr;
    Hit hit(Point::none, Vector3D::zero, -1);
    float dist = -1;
    for (auto &o : objects) {
        Hit temp_hit = o->mesh_filter->intersection(ray);
        float now = std::get<float>(temp_hit);

        if (fequal(now, -1)) continue;

        if (fequal(dist, -1) || now < dist) {
            if (change_hit_object) hit_object = o;
            hit = temp_hit;
            dist = now;
        }
    }

    return hit;
}

Vector3D Scene::rayTrace(Ray &ray, int depth) {
    if (depth > Scene::maxdepth) return Vector3D();

    // calculate the nearest hit
    Hit hit = getIntersection(ray);

    // no intersection point, return background
    if (fequal(std::get<float>(hit), -1)) return background;

    // local color(use blinn-phong model)
    Vector3D color;
    color = ambient_light->getColor(shared_from_this(), hit, ray.dir);
    for (auto &l : lights) {
        color = color + l->getColor(shared_from_this(), hit, ray.dir);
    }

    auto type = hit_object->mesh_renderer.material->type;
    Vector3D hit_normal = std::get<Vector3D>(hit);

    if (type != Material::Type::ROUGH) {
        Point hit_point = std::get<Point>(hit);
        Vector3D F0 = hit_object->mesh_renderer.material->F0;
        float n = hit_object->mesh_renderer.material->n;
        float cos_val = -Vector3D::dot(ray.dir, hit_normal);
        bool back_side = cos_val < 0;
        if (back_side) {
            cos_val = -cos_val;
            hit_normal = hit_normal * -1;
        }

        Vector3D F = F0 + (Vector3D(1, 1, 1) - F0) * std::pow(1 - cos_val, 5); // common version
        // Vector3D F = F0 + (Vector3D(1, 1, 1) - F0) * std::pow(2, (-5.55473 * cos_val - 6.98316) * cos_val); // UE4 version, said to be faster, but slower in test

        // reflected_color
        Vector3D reflect_dir = ray.dir + 2 * cos_val * hit_normal;
        Ray reflected_ray(hit_point, reflect_dir);
        color = color + F * rayTrace(reflected_ray, depth + 1);

        // refracted_color
        if (type == Material::Type::REFRACTIVE) {
            float ratio = back_side ? n / Material::n_air : Material::n_air / n;
            float temp = 1 - ratio * ratio * (1 - cos_val * cos_val);
            if (temp >= 0) {
                Vector3D refract_dir = ratio * (ray.dir + cos_val * hit_normal) - std::sqrt(temp) * hit_normal;
                Ray refracted_ray(hit_point, refract_dir);
                color = color + (Vector3D(1, 1, 1) - F) * rayTrace(refracted_ray, depth + 1);
            }
        }
    }

    return color;
}

void Scene::render(unsigned char *pixel, int windowWidth, int windowHeight) {
    camera->setPerspective(windowWidth, windowHeight);

    for (int x = 0; x < windowWidth; x++) {
        for (int y = 0; y < windowHeight; y++) {
            Ray ray = camera->getRay(x, y, windowWidth, windowHeight);
            Vector3D color = rayTrace(ray, 0);

            // write pixel
            int offset = y * windowWidth * 3 + x * 3;
            *(pixel + offset) = std::min(1.0f, color.x) * 255;
            *(pixel + offset + 1) = std::min(1.0f, color.y) * 255;
            *(pixel + offset + 2) = std::min(1.0f, color.z) * 255;
        }
    }
}