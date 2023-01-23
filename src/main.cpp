#include <iostream>
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

    auto light1 = std::make_shared<PointLight>(Vector3D(1, 1, 1) * 0.4, Point(0, 0, 5));
    auto light2 = std::make_shared<PointLight>(Vector3D(0.9, 0.9, 0.9) * 0.4, Point(4, 4, 4));
    auto light3 = std::make_shared<PointLight>(Vector3D(0.9, 0.9, 0.9) * 0.4, Point(3, -3, 1));

    auto sphere1 = std::make_shared<Object>();
    sphere1->mesh_filter = std::make_shared<Sphere>(Point(0, -2, 1), 1);
    sphere1->mesh_renderer.material = std::make_shared<Material>(Vector3D(0, 1, 0), 0.8, 0.2, 32);

    auto sphere2 = std::make_shared<Object>();
    sphere2->mesh_filter = std::make_shared<Sphere>(Point(-0.2, 1.5, 0.8), 0.8);
    sphere2->mesh_renderer.material = std::make_shared<Material>(Vector3D(1, 0, 0), 0.3, 0.7, 128);

    auto sphere3 = std::make_shared<Object>();
    sphere3->mesh_filter = std::make_shared<Sphere>(Point(-3, 0, 1.5), 1.5);
    sphere3->mesh_renderer.material = std::make_shared<Material>(Vector3D(0.86, 0.86, 0.86), 0.4, 0.8, 128, Vector3D(0.65, 0.65, 0.65));

    auto sphere4 = std::make_shared<Object>();
    sphere4->mesh_filter = std::make_shared<Sphere>(Point(3, 0, 0.5), 0.5);
    sphere4->mesh_renderer.material = std::make_shared<Material>(Vector3D(1, 1, 1), 0.35, 0.8, 128, Vector3D(0.02, 0.02, 0.02), 1.5);

    auto plane1 = std::make_shared<Object>();
    plane1->mesh_filter = std::make_shared<Plane>(Point(-10, -10, 0), Vector3D(20, 0, 0), Vector3D(0, 20, 0));
    plane1->mesh_renderer.material = std::make_shared<Material>(Vector3D(0.8, 0.8, 0.8), 0.7, 0.5, 32);

    auto plane2 = std::make_shared<Object>();
    plane2->mesh_filter = std::make_shared<Plane>(Point(3, 0, 0), Vector3D(0, 20, 0), Vector3D(0, 0, 20));
    plane2->mesh_renderer.material = std::make_shared<Material>(Vector3D(0.8, 0.8, 0.8), 0.7, 0.5, 32, Vector3D(0.08, 0.08, 0.08), 1.5);

    // OBJ obj1("D:\\DevelopTools\\Projects\\VSCode-projects\\C++\\graphics\\experiment\\ray-tracing\\modelSequence\\cactus\\cactus_0.obj");
    OBJ obj1("D:\\DevelopTools\\Projects\\VSCode-projects\\C++\\graphics\\experiment\\ray-tracing\\modelSequence\\cactus\\cactus_0.obj");
    auto model1 = std::make_shared<Object>();
    model1->mesh_filter = std::make_shared<Model>(obj1);
    model1->mesh_renderer.material = std::make_shared<Material>(Vector3D(1, 1, 1), 0.8, 0.5, 64);

    auto model2 = std::make_shared<Object>();
    auto m = std::make_shared<Model>();
    Point p1(0.29, 0, 0.866);
    Point p2(0.866, 0, 0);
    Point p3(0, 0.5, 0);
    Point p4(0, -0.5, 0);
    Face f1(3); f1.vertex.emplace_back(p1); f1.vertex.emplace_back(p2); f1.vertex.emplace_back(p3);
    Face f2(3); f2.vertex.emplace_back(p1); f2.vertex.emplace_back(p3); f2.vertex.emplace_back(p4);
    Face f3(3); f3.vertex.emplace_back(p1); f3.vertex.emplace_back(p4); f3.vertex.emplace_back(p2);
    Face f4(3); f4.vertex.emplace_back(p2); f4.vertex.emplace_back(p4); f4.vertex.emplace_back(p3);
    m->face.emplace_back(f1);
    m->face.emplace_back(f2);
    m->face.emplace_back(f3);
    m->face.emplace_back(f4);
    model2->mesh_filter = m;
    model2->mesh_renderer.material = std::make_shared<Material>(Vector3D(1, 1, 0), 0.4, 0.9, 128, Vector3D(0.85, 0.83, 0.79));

    s = std::make_shared<Scene>();
    // lights
    s->ambient_light = std::make_shared<AmbientLight>(Vector3D(0.1, 0.1, 0.1));
    s->addLight(light1);
    s->addLight(light2);
    s->addLight(light3);

    // objects
    // s->addObject(sphere1);
    // s->addObject(sphere2);
    // s->addObject(sphere3);
    // s->addObject(sphere4);
    // s->addObject(plane1);
    // s->addObject(plane2);
    s->addObject(model1);
    // s->addObject(model2);

    // camera
    s->camera = std::make_shared<Camera>();
    s->camera->setCamera(Point(2, 1, 0.6), Point(0, 1, 0.6), Vector3D::up, 60);

    // background
    // s->background = Vector3D(0.53, 0.81, 0.92);
    s->background = Vector3D(0.3, 0.3, 0.3);
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