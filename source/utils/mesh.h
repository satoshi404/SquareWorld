#pragma once
#include "shape.h"
#include <string>
#include <vector>

class Mesh : public Shape {
private:
    std::string objPath;

public:
    Mesh(const std::string& path);
    void init() override;
    std::string getObjPath() const { return objPath; }
};
