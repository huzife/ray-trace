#include "obj.h"

OBJ::OBJ(std::string path, float x_offs, float y_offs, float z_offs, float scale) {
    std::ifstream ifs(path, std::ios::in);
    if (!ifs.is_open()) {
        std::cerr << "Failed to load obj: " << path << std::endl;
        return;
    }

    std::string line;
    std::stringstream ss;
    while (std::getline(ifs, line)) {
        if (line.empty() || line[0] == '#') continue;

        ss.str("");
        ss << line;
        std::string type;
        ss >> type;

        if (type == "v") {
            float x, y, z;
            ss >> x >> y >> z;
            vertex.emplace_back(x * scale + x_offs, y * scale + y_offs, z * scale + z_offs);
        }
        else if (type == "f") {
            int idx;
            std::vector<int> v;
            while (ss >> idx) {
                v.emplace_back(idx - 1);
            }

            Face f(v.size());
            for (auto i : v) {
                f.vertex.emplace_back(vertex[i]);
            }
            face.emplace_back(f);
        }

        ss.clear();
    }
    ifs.close();
}