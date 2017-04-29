#pragma once

class Vector2 {
public:
    GLfloat x, y;

    Vector2() {
        Set(0.0f);
    }
    Vector2(GLfloat x, GLfloat y) {
        Set(x, y);
    }

    void Set(GLfloat v) {
        this->x = v;
        this->y = v;
    }
    void Set(GLfloat x, GLfloat y) {
        this->x = x;
        this->y = y;
    }
    void Increment(GLfloat v) {
        this->x += v;
        this->y += v;
    }
    void Increment(GLfloat x, GLfloat y) {
        this->x += x;
        this->y += y;
    }
};

class Vector3 {
public:
    GLfloat x, y, z;

    Vector3() {
        Set(0.0f);
    }
    Vector3(GLfloat x, GLfloat y, GLfloat z) {
        Set(x, y, z);
    }

    void Set(GLfloat v) {
        this->x = v;
        this->y = v;
        this->z = v;
    }
    void Set(GLfloat x, GLfloat y, GLfloat z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }
    void Increment(GLfloat v) {
        this->x += v;
        this->y += v;
        this->z += v;
    }
    void Increment(GLfloat x, GLfloat y, GLfloat z) {
        this->x += x;
        this->y += y;
        this->z += z;
    }
};
