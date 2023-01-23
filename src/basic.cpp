#include "basic.h"

bool fequal(float a, float b) {
    return std::abs(a - b) < FLOAT_EPSILON;
}

// basic vectors
const Vector3D Vector3D::zero(0, 0, 0);
const Vector3D Vector3D::right(1, 0, 0);
const Vector3D Vector3D::left(-1, 0, 0);
const Vector3D Vector3D::up(0, 1, 0);
const Vector3D Vector3D::down(0, -1, 0);
const Vector3D Vector3D::back(0, 0, 1);
const Vector3D Vector3D::forward(0, 0, -1);

std::ostream &operator<<(std::ostream &out, const Vector3D &v) {
    out << '(' << v.x << ", " << v.y << ", " << v.z << ')';
    return out;
}

Vector3D Vector3D::operator+(const Vector3D &v) const {
    return Vector3D(x + v.x, y + v.y, z + v.z);
}

Vector3D Vector3D::operator-(const Vector3D &v) const {
    return Vector3D(x - v.x, y - v.y, z - v.z);
}

Vector3D Vector3D::operator*(float val) const {
    return Vector3D(x * val, y * val, z * val);
}

Vector3D Vector3D::operator*(const Vector3D &v) const {
    return Vector3D(x * v.x, y * v.y, z * v.z);
}

Vector3D operator*(float val, const Vector3D &v) {
    return v * val;
}

Vector3D Vector3D::operator/(float val) const {
    return Vector3D(x / val, y / val, z / val);
}

bool Vector3D::operator==(const Vector3D &v) const {
    return fequal(x, v.x) && fequal(y, v.y) && fequal(z, v.z);
}

bool Vector3D::operator!=(const Vector3D &v) const {
    return !fequal(x, v.x) || !fequal(y, v.y) || !fequal(z, v.z);
}

float Vector3D::dot(const Vector3D &v) const {
    return x * v.x + y * v.y + z * v.z;
}

float Vector3D::dot(const Vector3D &v1, const Vector3D &v2) {
    return v1.dot(v2);
}

Vector3D Vector3D::cross(const Vector3D &v) const {
    Vector3D result;
    result.x = y * v.z - z * v.y;
    result.y = z * v.x - x * v.z;
    result.z = x * v.y - y * v.x;

    return result;
}

Vector3D Vector3D::cross(const Vector3D &v1, const Vector3D &v2) {
    return v1.cross(v2);
}

void Vector3D::normalize() {
    float len = magnitude();
    x /= len;
    y /= len;
    z /= len;
}

Vector3D Vector3D::normalized() const {
    float len = magnitude();
    return Vector3D(x / len, y / len, z / len);
}

float Vector3D::sqrMagnitude() const {
    return x * x + y * y + z * z;
}

float Vector3D::magnitude() const {
    return std::sqrt(sqrMagnitude());
}

float Vector3D::angle(const Vector3D &v1, const Vector3D &v2) {
    float dot_ret = Vector3D::dot(v1, v2);
    float lens = v1.magnitude() * v2.magnitude();
    return Angle::radToDeg(std::acos(dot_ret / lens));
}

float Vector3D::signedAngle(const Vector3D &from, const Vector3D &to, const Vector3D &dir) {
    Vector3D n = Vector3D::cross(from, to); // calculate the normal vector in order: from -> to
    float check_value = Vector3D::dot(dir, n);
    if (fequal(check_value, 0)) assert(false); // direction vector and plane(from, to) are coplanar

    float ret = Vector3D::angle(from, to);
    if (check_value < 0) ret *= -1;

    return ret;
}

// Angle
float Angle::degToRad(float deg) {
    return deg * std::numbers::pi / 180;
}

float Angle::radToDeg(float rad) {
    return rad * 180 / std::numbers::pi;
}

// Point
// basic points
const Point Point::zero(0, 0, 0);
const Point Point::none(true);

std::ostream &operator<<(std::ostream &out, const Point &v) {
    if (v.is_none)
        out << "none";
    else
        out << '(' << v.x << ", " << v.y << ", " << v.z << ')';
    return out;
}

Point Point::operator+(const Vector3D &v) const {
    assert(!is_none);
    return Point(x + v.x, y + v.y, z + v.z);
}

Point operator+(const Vector3D &v, const Point &p) {
    assert(!p.is_none);
    return p + v;
}

Vector3D Point::operator-(const Point &p) const {
    assert(!is_none && !p.is_none);
    return Vector3D(x - p.x, y - p.y, z - p.z);
}

Point Point::operator-(const Vector3D &v) const {
    return Point(x - v.x, y - v.y, z - v.z);
}

bool Point::operator==(const Point &p) const {
    assert(!is_none && !p.is_none);
    return fequal(x, p.x) && fequal(y, p.y) && fequal(z, p.z);
}

bool Point::operator!=(const Point &p) const {
    assert(!is_none && !p.is_none);
    return !fequal(x, p.x) || !fequal(y, p.y) || !fequal(z, p.z);
}

float Point::distance(const Point &p1, const Point &p2) {
    assert(!p1.is_none && !p2.is_none);
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    float dz = p2.z - p1.z;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

// Face
Vector3D Face::normal() {
    Vector3D v1 = vertex[1] - vertex[0];
    Vector3D v2 = vertex[2] - vertex[0];
    return Vector3D::cross(v1, v2).normalized();
}