#include <iostream>
#include <memory>
#include <vector>
#include "GL/freeglut.h"
#include "basic.h"
#include "mesh.h"
#include "objects.h"
#include "obj.h"

const int windowWidth = 1280;
const int windowHeight = 720;

std::shared_ptr<Scene> s;

void init() {
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    auto light1 = std::make_shared<PointLight>(Vector3D(1, 1, 1) * 0.55, Point(0, 0, 5));
    auto light2 = std::make_shared<PointLight>(Vector3D(0.9, 0.9, 0.9) * 0.55, Point(4, 4, 4));

    auto sphere1 = std::make_shared<Object>();
    sphere1->mesh_filter = std::make_shared<Sphere>(Point(-2, -2, 1.5), 1.5);
    sphere1->mesh_renderer.material = std::make_shared<Material>(Vector3D(0, 1, 0), 0.8, 0.2, 32, 0);

    auto sphere2 = std::make_shared<Object>();
    sphere2->mesh_filter = std::make_shared<Sphere>(Point(-3, 2, 1.8), 1.8);
    sphere2->mesh_renderer.material = std::make_shared<Material>(Vector3D(0.86, 0.86, 0.86), 0.4, 0.8, 128, 1, Vector3D(0.65, 0.65, 0.65));

    auto sphere3 = std::make_shared<Object>();
    sphere3->mesh_filter = std::make_shared<Sphere>(Point(0.5, -2.5, 0.7), 0.7);
    sphere3->mesh_renderer.material = std::make_shared<Material>(Vector3D(1, 0, 0), 0.6, 0.7, 128, 0);

    auto sphere4 = std::make_shared<Object>();
    sphere4->mesh_filter = std::make_shared<Sphere>(Point(1.5, -0.1, 0.75), 0.75);
    sphere4->mesh_renderer.material = std::make_shared<Material>(Vector3D(1, 1, 1), 0.35, 0.8, 128, 0, Vector3D(0.02, 0.02, 0.02), 1.015);

    std::vector<std::shared_ptr<Object>> plane(6);
    for (auto &p : plane) p = std::make_shared<Object>();
    plane[0]->mesh_filter = std::make_shared<Plane>(Point(-10, -10, 0), Vector3D(20, 0, 0), Vector3D(0, 20, 0));
    plane[0]->mesh_renderer.material = std::make_shared<Material>(Vector3D(0.8, 0.8, 0.8), 0.7, 0.5, 32, 0, Vector3D(0.02, 0.02, 0.02));

    plane[1]->mesh_filter = std::make_shared<Plane>(Point(-10, -10, 6), Vector3D(0, 20, 0), Vector3D(20, 0, 0));
    plane[1]->mesh_renderer.material = std::make_shared<Material>(Vector3D(1, 1, 1), 1, 0.1, 1, 0);

    plane[2]->mesh_filter = std::make_shared<Plane>(Point(-10, -10, 0), Vector3D(0, 20, 0), Vector3D(0, 0, 6));
    plane[2]->mesh_renderer.material = std::make_shared<Material>(Vector3D(0.5, 0.5, 0.79), 0.8, 0.3, 32, 0);

    plane[3]->mesh_filter = std::make_shared<Plane>(Point(10, -10, 0), Vector3D(-20, 0, 0), Vector3D(0, 0, 6));
    plane[3]->mesh_renderer.material = std::make_shared<Material>(Vector3D(0.8, 0.6, 0.8), 0.8, 0.3, 32, 0);

    plane[4]->mesh_filter = std::make_shared<Plane>(Point(-10, 10, 0), Vector3D(20, 0, 0), Vector3D(0, 0, 6));
    plane[4]->mesh_renderer.material = std::make_shared<Material>(Vector3D(0.8, 0.6, 0.8), 0.8, 0.3, 32, 0);
    
    plane[5]->mesh_filter = std::make_shared<Plane>(Point(10, -10, 0), Vector3D(0, 0, 6), Vector3D(0, 20, 0));
    plane[5]->mesh_renderer.material = std::make_shared<Material>(Vector3D(0.5, 0.5, 0.79), 0.8, 0.3, 32, 0);

    auto water = std::make_shared<Object>();
    water->mesh_filter = std::make_shared<Plane>(Point(-10, -10, 0.4), Vector3D(20, 0, 0), Vector3D(0, 20, 0));
    water->mesh_renderer.material = std::make_shared<Material>(Vector3D(0.68, 0.87, 0.89), 0.4, 0.5, 32, 0, Vector3D(0.02, 0.02, 0.02), 1.33);

    auto wall = std::make_shared<Object>();
    wall->mesh_filter = std::make_shared<Plane>(Point(-5, -10, 0), Vector3D(0, 20, 0), Vector3D(0, 0, 20));
    wall->mesh_renderer.material = std::make_shared<Material>(Vector3D(1, 1, 1), 0.4, 0.5, 32, 0, Vector3D(0.02, 0.02, 0.02), 1);

    OBJ obj1("../model/model.obj", 1, 2, 0, 1.75);
    auto model1 = std::make_shared<Object>();
    model1->mesh_filter = std::make_shared<Model>(obj1);
    model1->mesh_renderer.material = std::make_shared<Material>(Vector3D(0.80, 0.69, 0.49), 0.8, 0.5, 64, 0);

    auto model2 = std::make_shared<Object>();
    auto m = std::make_shared<Model>();
    Point p1(0.39, -2, 1.1);
    Point p2(1.1, -2, 0);
    Point p3(0, -1.25, 0);
    Point p4(0, -2.75, 0);
    Face f1(3); f1.vertex.emplace_back(p1); f1.vertex.emplace_back(p2); f1.vertex.emplace_back(p3);
    Face f2(3); f2.vertex.emplace_back(p1); f2.vertex.emplace_back(p3); f2.vertex.emplace_back(p4);
    Face f3(3); f3.vertex.emplace_back(p1); f3.vertex.emplace_back(p4); f3.vertex.emplace_back(p2);
    Face f4(3); f4.vertex.emplace_back(p2); f4.vertex.emplace_back(p4); f4.vertex.emplace_back(p3);
    m->face.emplace_back(f1);
    m->face.emplace_back(f2);
    m->face.emplace_back(f3);
    m->face.emplace_back(f4);
    model2->mesh_filter = m;
    model2->mesh_renderer.material = std::make_shared<Material>(Vector3D(1, 1, 0), 0.7, 0.9, 128, 0.8, Vector3D(0.85, 0.83, 0.79));

    s = std::make_shared<Scene>();
    // lights
    s->ambient_light = std::make_shared<AmbientLight>(Vector3D(0.1, 0.1, 0.1));
    s->addLight(light1);
    s->addLight(light2);

    // objects
    s->addObject(sphere1);
    s->addObject(sphere2);
    s->addObject(sphere3);
    s->addObject(sphere4);
    s->addObject(plane[0]);
    s->addObject(plane[1]);
    s->addObject(plane[2]);
    s->addObject(plane[3]);
    s->addObject(plane[4]);
    s->addObject(plane[5]);
    // s->addObject(water);
    // s->addObject(wall);
    
    s->addObject(model1);
    // s->addObject(model2);

    // camera
    s->camera = std::make_shared<Camera>();
    s->camera->setCamera(Point(5, 0, 1), Point(0, 0, 0.5), Vector3D::back, 60);

    // background
    // s->background = Vector3D(0.53, 0.81, 0.92);
    s->background = Vector3D(0, 0, 0);
}

bool done = false;
void display() {
    if (done) return;
    done = true;
    auto start = std::chrono::steady_clock::now();
    GLubyte *pixel = new GLubyte[windowWidth * windowHeight * 3];

    s->render(pixel, windowWidth, windowHeight);
    glDrawPixels(windowWidth, windowHeight, GL_RGB, GL_UNSIGNED_BYTE, pixel);
    glutSwapBuffers();
    delete[] pixel;

    auto end = std::chrono::steady_clock::now();
    auto time = std::chrono::duration<double, std::milli>(end - start).count();
    std::cout << time << std::endl;
}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("test");
    init();
    glutDisplayFunc(display);
    glutMouseFunc([](int, int, int, int){});
    glutMainLoop();
    return 0;
}