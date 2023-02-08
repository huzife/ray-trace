#ifndef _BASIC_H
#define _BASIC_H

#include <iostream>
#include <cmath>
#include <numbers>
#include <vector>
#include <tuple>
#include <memory>
#include <assert.h>
#include <chrono>

#define FLOAT_EPSILON 1e-6f

bool fequal(float a, float b);

class Vector3D {
public:
    // basic vectors
    static const Vector3D zero;
    static const Vector3D right;
    static const Vector3D left;
    static const Vector3D up;
    static const Vector3D down;
    static const Vector3D back;
    static const Vector3D forward;

public:
    float x;
    float y;
    float z;

    // construct function
    Vector3D() : x(0.0f), y(0.0f), z(0.0f) {}
    Vector3D(float x, float y, float z) : x(x), y(y), z(z) {}
    Vector3D(const Vector3D &) = default;
    Vector3D &operator=(const Vector3D &) = default;

    // print function
    friend std::ostream &operator<<(std::ostream &out, const Vector3D &v);

    // operations
    Vector3D operator+(const Vector3D &v) const; // vector + vector

    Vector3D operator-(const Vector3D &v) const; // vector - vector

    Vector3D operator*(float val) const;                     // vector * scalar
    friend Vector3D operator*(float val, const Vector3D &v); // scalar * vector
    Vector3D operator*(const Vector3D &v) const;             // multiply each component

    Vector3D operator/(float val) const; // vector / scalar

    bool operator==(const Vector3D &v) const; // equal operator
    bool operator!=(const Vector3D &v) const; // non-equal operator

    float dot(const Vector3D &v) const; // inner product
    static float dot(const Vector3D &v1, const Vector3D &v2);

    Vector3D cross(const Vector3D &v) const; // outer product
    static Vector3D cross(const Vector3D &v1, const Vector3D &v2);

    void normalize();            // normalize the vector itself
    Vector3D normalized() const; // return the normalized vector

    float sqrMagnitude() const; // return the square of the length
    float magnitude() const;    // return the length

    static float angle(const Vector3D &v1, const Vector3D &v2);                              // calculate the angle between two vectors (0 to 180)
    static float signedAngle(const Vector3D &from, const Vector3D &to, const Vector3D &dir); // calculate the angle between two vectors (-180 to 180)

    static Vector3D mix(const Vector3D &v1, const Vector3D &v2, float p); // linear mix algorithm
};

// this is a utils class, help to implement some operation of angle
class Angle {
public:
    static float degToRad(float deg);
    static float radToDeg(float rad);
};

class Point {
public:
    static const Point zero;
    static const Point none;

public:
    float x;
    float y;
    float z;

    bool is_none;

    // construct function
    Point(bool n = false) : x(0.0f), y(0.0f), z(0.0f), is_none(n) {}
    Point(float x, float y, float z) : x(x), y(y), z(z), is_none(false) {}
    Point(const Point &) = default;
    Point &operator=(const Point &) = default;

    // print function
    friend std::ostream &operator<<(std::ostream &out, const Point &v);

    // operations
    Point operator+(const Vector3D &v) const;                  // point + vector
    friend Point operator+(const Vector3D &v, const Point &p); // vector + point, equal to point + vector

    Vector3D operator-(const Point &p) const; // point - point
    Point operator-(const Vector3D &v) const; // point - vector

    bool operator==(const Point &p) const; // equal operator
    bool operator!=(const Point &p) const; // non-equal operator

    static float distance(const Point &p1, const Point &p2);
};

class Face {
public:
    int v_counts;
    std::vector<Point> vertex;

    Face(int v) : v_counts(v) { assert(v >= 3); }
    Face(const Face &) = default;
    Face &operator=(const Face &) = default;

    Vector3D normal();
};

class Ray {
public:
    static constexpr float offset = 0.0001f;

public:
    Point start;
    Vector3D dir;

    Ray() = default;
    Ray(const Point &p, const Vector3D &v) : start(p), dir(v.normalized()) {}
    Ray(const Ray &) = default;
    Ray &operator=(const Ray &) = default;
};

using Hit = std::tuple<Point, Vector3D, float>;

#endif // _BASIC_H