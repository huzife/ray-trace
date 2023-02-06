#ifndef _OBJ_H
#define _OBJ_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include "basic.h"

class OBJ {
public:
    std::vector<Point> vertex;
    std::vector<Face> face;

    OBJ(std::string path, float x_offs = 0, float y_offs = 0, float z_offs = 0, float scale = 1);
};

#endif // _OBJ_H