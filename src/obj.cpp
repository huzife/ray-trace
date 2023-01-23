#include "obj.h"

OBJ::OBJ(std::string path) {
    std::ifstream ifs(path, std::ios::in);
    if (ifs.is_open()) {
        std::string line;
        std::stringstream ss;
        int cnt = 100;
        while (std::getline(ifs, line)) {
            if (line.empty() || line[0] == '#') continue;

            ss.str("");
            ss << line;
            std::string type;
            ss >> type;

            if (type == "v") {
                float x, y, z;
                ss >> x >> y >> z;
                vertex.emplace_back(x, y, z);
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
                if (--cnt == 0) break;
            }

            ss.clear();
        }
    }
    ifs.close();
}