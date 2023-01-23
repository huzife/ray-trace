#include "mesh.h"

/**
 *  sphere: (x - cx)^2 + (y - cy)^2 + (z - cz)^2 = r^2
 *  ray:    P(t) = start + t * dir(normalized)
 *
 *  -> At^2 + Bt + C = 0
 */
Hit Sphere::intersection(const Ray &ray) {
    float xd = ray.dir.x, yd = ray.dir.y, zd = ray.dir.z;
    float xp = ray.start.x, yp = ray.start.y, zp = ray.start.z;
    float cx = center.x, cy = center.y, cz = center.z;
    // float A = ray.dir.sqrMagnitude(); // calculator A, actually A == 1 because ray.dir is normalized
    float B = 2 * (xd * (xp - cx) + yd * (yp - cy) + zd * (zp - cz));
    float C = (xp - cx) * (xp - cx) + (yp - cy) * (yp - cy) + (zp - cz) * (zp - cz) - radius * radius;
    float delta = B * B - 4 * C;

    // no intersection point
    if (delta < 0) return Hit(Point::none, Vector3D::zero, -1);

    delta = std::sqrt(delta);
    float t1 = (-B + delta) / 2.0f;
    float t2 = (-B - delta) / 2.0f;

    // t < 0 means the intersection point is in the opposite side of the ray
    if (t1 < Ray::offset) return Hit(Point::none, Vector3D::zero, -1);
    float t = t2 < Ray::offset ? t1 : t2;

    Point point = ray.start + t * ray.dir;
    Vector3D dir = (point - center).normalized();

    return Hit(point, dir, t);
}

/**
 *  face: Ax + By + Cz + d = 0, n(normal) = (A, B, C)
 *  ray:    P(t) = start + t * dir(normalized)
 *
 *  -> t = (start - P0) dot n / dir dot n, P0 is one of face's points
 */
Hit Plane::intersection(const Ray &ray) {
    Vector3D n = Vector3D::cross(right, up).normalized();
    float divisor = Vector3D::dot(ray.dir, n);

    // check if the ray and the face are parallel
    if (fequal(divisor, 0)) return Hit(Point::none, Vector3D::zero, -1);

    // t < 0 means the intersection point is in the opposite side of the ray
    float t = -Vector3D::dot(ray.start - lb, n) / divisor;
    if (t < Ray::offset) return Hit(Point::none, Vector3D::zero, -1);

    Point hit_point = ray.start + t * ray.dir;

    // check if the point is in the plane
    Vector3D v = hit_point - lb;
    float up_len = up.magnitude();
    float right_len = right.magnitude();

    // check up direction
    float len = Vector3D::dot(v, up) / up_len;
    if (len < 0 || len > up_len) return Hit(Point::none, Vector3D::zero, -1);

    // check right direction
    len = Vector3D::dot(v, right) / right_len;
    if (len < 0 || len > right_len) return Hit(Point::none, Vector3D::zero, -1);
    return Hit(hit_point, n, t);
}

Hit Model::intersection(const Ray &ray) {
    Hit hit(Point::none, Vector3D::zero, -1);
    float dist = -1;
    bool first_hit = false;
    for (auto &f : face) {
        Vector3D n = f.normal();
        float divisor = Vector3D::dot(ray.dir, n);

        // check if the ray and the face are parallel
        if (fequal(divisor, 0)) continue;

        // t < 0 means the intersection point is in the opposite side of the ray
        float t = -Vector3D::dot(ray.start - f.vertex[0], n) / divisor;
        if (t < Ray::offset || first_hit && dist < t) continue;

        // check if the point is in the face
        Point hit_point = ray.start + t * ray.dir;
        bool inside = true;
        for (int i = 0; i < f.v_counts; i++) {
            Vector3D v = hit_point - f.vertex[i];
            int last = (i + f.v_counts - 1) % f.v_counts;
            int next = (i + 1) % f.v_counts;
            Vector3D v1 = f.vertex[next] - f.vertex[i];
            Vector3D v2 = f.vertex[last] - f.vertex[i];
            if (Vector3D::dot(Vector3D::cross(v1, v), Vector3D::cross(v1, v2)) < 0) {
                inside = false;
                break;
            }
        }

        if (!inside) continue;
        hit = Hit(hit_point, n, t);
        dist = t;
        if (!first_hit) first_hit = true;
    }

    return hit;
}