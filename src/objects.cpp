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

void Light::setParentScene(std::shared_ptr<Scene> scene) {
    parent_scene = scene;
}

Vector3D AmbientLight::getColor(const Hit &hit, std::shared_ptr<Object> hit_object, const Vector3D &V) {
    if (Vector3D::dot(V, std::get<Vector3D>(hit)) > 0) return Vector3D::zero;
    Vector3D ret = hit_object->mesh_renderer.material->Ka * intensity;

    return ret;
}

Vector3D PointLight::getColor(const Hit &hit, std::shared_ptr<Object> hit_object, const Vector3D &V) {
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
    if (parent_scene->underShadow(detect_ray, t_max)) return Vector3D::zero;

    // calculate cosA, A is the angle of L and n
    float a = Vector3D::dot(L, hit_normal);
    if (a <= 0) return Vector3D::zero;

    // calculate cosB, B is the angle of H and n, H is the half-range vector of L and V
    Vector3D H = L - V;
    float b = Vector3D::dot(H, hit_normal) / H.magnitude();

    // diffuse and reflect
    auto material = hit_object->mesh_renderer.material;
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
    light->setParentScene(shared_from_this());
}

void Scene::delLight(std::shared_ptr<Light> light) {
    assert(std::dynamic_pointer_cast<AmbientLight>(light) == nullptr);
    lights.erase(light);
}

HitInfo Scene::getIntersection(Ray &ray) {
    // calculate the nearest hit
    std::shared_ptr<Object> hit_object = nullptr;
    Hit hit(Point::none, Vector3D::zero, -1);
    float dist = -1;
    for (auto &o : objects) {
        Hit temp_hit = o->mesh_filter->intersection(ray);
        float now = std::get<float>(temp_hit);

        if (fequal(now, -1)) continue;

        if (fequal(dist, -1) || now < dist) {
            hit_object = o;
            hit = temp_hit;
            dist = now;
        }
    }

    return {hit, hit_object};
}

bool Scene::underShadow(Ray &ray, float t_max) {
    Hit detect_hit = getIntersection(ray).first;
    float t_hit = std::get<float>(detect_hit);

    return !fequal(t_hit, -1) && t_hit < t_max;
}

Vector3D Scene::rayTrace(Ray &ray, int depth) {
    if (depth > Scene::maxdepth) return Vector3D();

    // calculate the nearest hit
    Hit hit;
    std::shared_ptr<Object> hit_object;
    std::tie(hit, hit_object) = getIntersection(ray);

    // no intersection point, return background
    if (fequal(std::get<float>(hit), -1)) return background;

    Vector3D color;

    // local color(use blinn-phong model)
    color = ambient_light->getColor(hit, hit_object, ray.dir);
    for (auto &l : lights) {
        color = color + l->getColor(hit, hit_object, ray.dir);
    }

    auto hit_material = hit_object->mesh_renderer.material;
    auto type = hit_material->type;
    // if the material is rough, return the local color
    if (type == Material::Type::ROUGH) return color;

    Point hit_point = std::get<Point>(hit);
    Vector3D hit_normal = std::get<Vector3D>(hit);
    Vector3D F0 = hit_material->F0;
    float cos_val = -Vector3D::dot(ray.dir, hit_normal);
    bool back_side = cos_val < 0;
    if (back_side) {
        cos_val = -cos_val;
        hit_normal = hit_normal * -1;
    }

    // common version
    Vector3D F = F0 + (Vector3D(1, 1, 1) - F0) * std::pow(1 - cos_val, 5);

    // UE4 version, said to be faster, but slower in test
    // Vector3D F = F0 + (Vector3D(1, 1, 1) - F0) * std::pow(2, (-5.55473 * cos_val - 6.98316) * cos_val);

    // reflected_color
    Vector3D reflect_dir = ray.dir + 2 * cos_val * hit_normal;
    Ray reflected_ray(hit_point, reflect_dir);
    color = color + F * rayTrace(reflected_ray, depth + 1);

    // refracted_color
    if (type != Material::Type::REFRACTIVE) return color;
    
    float n = hit_material->n;
    float ratio = back_side ? n / Material::n_air : Material::n_air / n;
    float temp = 1 - ratio * ratio * (1 - cos_val * cos_val);
    if (temp >= 0) {
        Vector3D refract_dir = ratio * (ray.dir + cos_val * hit_normal) - std::sqrt(temp) * hit_normal;
        Ray refracted_ray(hit_point, refract_dir);
        color = color + (Vector3D(1, 1, 1) - F) * rayTrace(refracted_ray, depth + 1);
    }

    return color;
}

void Scene::render(unsigned char *pixel, int windowWidth, int windowHeight) {
    camera->setPerspective(windowWidth, windowHeight);

#ifdef MULTI_THREADS
    std::cout << "enable multi-threads" << std::endl;
    int w = windowWidth / 4;
    int h = windowHeight / 2;
    std::vector<std::thread> threads;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 2; j++) {
            std::thread t([&, i, j]() {
                for (int x = i * w; x < (i + 1) * w; x++) {
                    for (int y = j * h; y < (j + 1) * h; y++) {
                        Ray ray = camera->getRay(x, y, windowWidth, windowHeight);
                        Vector3D color = rayTrace(ray, 0);

                        // // write pixel
                        int offset = y * windowWidth * 3 + x * 3;
                        *(pixel + offset) = std::min(1.0f, color.x) * 255;
                        *(pixel + offset + 1) = std::min(1.0f, color.y) * 255;
                        *(pixel + offset + 2) = std::min(1.0f, color.z) * 255;
                    }
                }
            });
            threads.push_back(std::move(t));
        }
    }

    for (auto &t : threads) t.join();
#else
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
#endif
}