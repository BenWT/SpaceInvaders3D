#pragma once

class Plane {
public:
    Mesh mesh;

    Plane() {}
    Plane(const char* path, GLfloat x, GLfloat y, GLfloat w, GLfloat h) {
        this->mesh = CreatePlane(path, w, h);
        this->Move(x, y);
        this->mesh.rotation.Increment(0.0f, 180.0f, 0.0f);
    }

    void Move(GLfloat x, GLfloat y) {
        this->mesh.position.Increment(x, y, 0.0f);
    }

    void Render(Shader &hud, glm::mat4 &projection) {
        mesh.RenderPlane(hud, projection);
    }

    Mesh CreatePlane(const char* path, GLfloat w, GLfloat h) {
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;
        GLuint texture;

        Vertex topL, topR, botL, botR;

        topL.texCoords = glm::vec2(-1.0f, 1.0f);
        topR.texCoords = glm::vec2(1.0f, 1.0f);
        botL.texCoords = glm::vec2(-1.0f, -1.0f);
        botR.texCoords = glm::vec2(1.0f, -1.0f);

        topL.position = glm::vec3(topL.texCoords.x, topL.texCoords.y, 10.0f);
        topR.position = glm::vec3(topR.texCoords.x, topR.texCoords.y, 10.0f);
        botL.position = glm::vec3(botL.texCoords.x, botL.texCoords.y, 10.0f);
        botR.position = glm::vec3(botR.texCoords.x, botR.texCoords.y, 10.0f);

        topL.normal = glm::vec3(0.0f, 0.0f, 0.0f);
        topR.normal = glm::vec3(0.0f, 0.0f, 0.0f);
        botL.normal = glm::vec3(0.0f, 0.0f, 0.0f);
        botR.normal = glm::vec3(0.0f, 0.0f, 0.0f);

        vertices.push_back(topL);
        vertices.push_back(topR);
        vertices.push_back(botL);
        vertices.push_back(botR);

        indices.push_back(2);
        indices.push_back(0);
        indices.push_back(1);

        indices.push_back(1);
        indices.push_back(3);
        indices.push_back(2);

        texture = loadTexture(path);

        return Mesh(vertices, indices, texture);
    }

    GLuint loadTexture(const char* path) {
        GLuint textureID;
        SDL_Surface* image = IMG_Load(path);

        glGenTextures(1, &textureID);

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->w, image->h, 0, GL_RGB, GL_UNSIGNED_BYTE, image->pixels);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
        SDL_FreeSurface(image);

        return textureID;
    }
};
