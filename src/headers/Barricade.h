#pragma once
#include "Model.h"

class Barricade : public Model {
public:
    int hits = 0;

    Barricade() {}
    Barricade(GLchar* path, GLchar* texture) {
        this->loadModel(path, texture);
    }
};
