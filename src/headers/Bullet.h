#pragma once
#include "Model.h"

class Bullet : public Model {
public:
    float speed = 4.0f;
    bool isPlayer;
    bool doDestroy = false;

    Bullet() {}
    Bullet(GLchar* path, GLchar* texture) {
        this->loadModel(path, texture);
    }

    void SetBullet(Vector3 startPos, bool isPlayer) {
        this->meshes[0].position = startPos;
        this->isPlayer = isPlayer;
    }

    void Update(GLfloat &deltaTime) {
        if (isPlayer) this->Move(0.0f, deltaTime * speed, 0.0f);
        else this->Move(0.0f, deltaTime * -speed / 2, 0.0f);

        if (this->getY() > 4.0f || this->getY() < -4.0f) doDestroy = true;
    }
};
