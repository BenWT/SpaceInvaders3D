#pragma once
#include "Vertex.h"
#include "Mesh.h"

class Model {
public:
    std::vector<Mesh> meshes;

    Model() {}
    Model(GLchar* path, GLchar* texture) {
        this->loadModel(path, texture);
    }

    void Render(Shader &shader, Camera &camera, glm::mat4 &projectionMat) {
        for(GLuint i = 0; i < this->meshes.size(); i++) {
            this->meshes[i].Render(shader, camera, projectionMat);
        }
    }
    void Render(Shader &shader, Camera &camera, glm::mat4 &projectionMat, int i) {
        this->meshes[i].Render(shader, camera, projectionMat);
    }

    void Move(GLfloat x, GLfloat y, GLfloat z) {
        for(GLuint i = 0; i < this->meshes.size(); i++) this->meshes[i].position.Increment(x, y, z);
    }
    void SetPosition(GLfloat x, GLfloat y, GLfloat z) {
        for(GLuint i = 0; i < this->meshes.size(); i++) this->meshes[i].position.Set(x, y, z);
    }

    void Rotate(GLfloat v) {
        for(GLuint i = 0; i < this->meshes.size(); i++) this->meshes[i].rotation.Increment(v);
    }
    void Rotate(GLfloat x, GLfloat y, GLfloat z) {
        for(GLuint i = 0; i < this->meshes.size(); i++) this->meshes[i].rotation.Increment(x, y, z);
    }
    void SetRotation(GLfloat x, GLfloat y, GLfloat z) {
        for(GLuint i = 0; i < this->meshes.size(); i++) this->meshes[i].rotation.Set(x, y, z);
    }

    void Scale(GLfloat v) {
        for(GLuint i = 0; i < this->meshes.size(); i++) this->meshes[i].scale.Set(v);
    }
    void Scale(GLfloat x, GLfloat y, GLfloat z) {
        for(GLuint i = 0; i < this->meshes.size(); i++) this->meshes[i].scale.Set(x, y, z);
    }
    void Grow(GLfloat v) {
        for(GLuint i = 0; i < this->meshes.size(); i++) this->meshes[i].scale.Increment(v);
    }

    void loadModel(std::string path, const char* texturePath) {
        Assimp::Importer import;
        const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_FixInfacingNormals | aiProcess_ConvertToLeftHanded);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << "ASSIMP Error: " << import.GetErrorString() << std::endl;
        } else {
            this->directory = path.substr(0, path.find_last_of('/'));
            this->processNode(scene->mRootNode, scene, texturePath);
        }
    }

    Vector3 getPosition() { return this->meshes[0].position; }
    GLfloat getX() { return this->meshes[0].position.x; }
    GLfloat getY() { return this->meshes[0].position.y; }
    GLfloat getZ() { return this->meshes[0].position.z; }

private:
    std::string directory;

    void processNode(aiNode* node, const aiScene* scene, const char* texturePath) {
        for (GLuint i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            this->meshes.push_back(this->processMesh(mesh, scene, texturePath));
        }

        for (GLuint i = 0; i < node->mNumChildren; i++) {
            this->processNode(node->mChildren[i], scene, texturePath);
        }
    }

    Mesh processMesh(aiMesh* mesh, const aiScene* scene, const char* texturePath) {
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;
        GLuint texture;

        for (GLuint i = 0; i < mesh->mNumVertices; i++) {
            Vertex vert;
            glm::vec3 vector;

            // Positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vert.position = vector;

            // Normals
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vert.normal = vector;

            // Texture Coordinates
            if (mesh->mTextureCoords[0]) {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vert.texCoords = vec;
            } else {
                vert.texCoords = glm::vec2(0.0f, 0.0f);
            }

            vertices.push_back(vert);
        }

        for (GLuint i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];

            for (GLuint j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        texture = loadTexture(texturePath);

        return Mesh(vertices, indices, texture);
    }

    GLuint loadTexture(const char* texturePath) {
        GLuint textureID;
        SDL_Surface* image = IMG_Load(texturePath);

        glGenTextures(1, &textureID);

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->w, image->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
        SDL_FreeSurface(image);

        return textureID;
    }
};
