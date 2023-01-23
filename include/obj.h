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

    OBJ(std::string path);
};

#endif // _OBJ_H