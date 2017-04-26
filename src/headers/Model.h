#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include "Vertex.h"
#include "Mesh.h"

class Model {
public:
    Model() {}
    Model(GLchar* path) {
        this->loadModel(path);
    }

    void Move() {

    }

    void Rotate(GLfloat v) {
        for(GLuint i = 0; i < this->meshes.size(); i++) {
            this->meshes[i].rotation.x += v;
            this->meshes[i].rotation.y += v;
            this->meshes[i].rotation.z += v;
        }
    }
    void Rotate(GLfloat x, GLfloat y, GLfloat z) {
        for(GLuint i = 0; i < this->meshes.size(); i++) {
            this->meshes[i].rotation.x += x;
            this->meshes[i].rotation.y += y;
            this->meshes[i].rotation.z += z;
        }
    }

    void Render(Shader shader, glm::mat4 &projectionMat, glm::mat4 &viewMat) {
        for(GLuint i = 0; i < this->meshes.size(); i++) {
            this->meshes[i].Render(shader, projectionMat, viewMat);
        }
    }

private:
    std::vector<Mesh> meshes;
    std::string directory;

    void loadModel(std::string path) {
        Assimp::Importer import;
        const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_FixInfacingNormals | aiProcess_ConvertToLeftHanded);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << "ASSIMP Error: " << import.GetErrorString() << std::endl;
        } else {
            this->directory = path.substr(0, path.find_last_of('/'));
            this->processNode(scene->mRootNode, scene);
        }
    }

    void processNode(aiNode* node, const aiScene* scene) {
        for (GLuint i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            this->meshes.push_back(this->processMesh(mesh, scene));
        }

        for (GLuint i = 0; i < node->mNumChildren; i++) {
            this->processNode(node->mChildren[i], scene);
        }
    }

    Mesh processMesh(aiMesh* mesh, const aiScene* scene) {
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;

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

        return Mesh(vertices, indices);
    }

    glm::vec3 newVec3(double x, double y, double z) {
        glm::vec3 vector;
        vector.x = x;
        vector.y = y;
        vector.z = z;
        return vector;
    }
};
