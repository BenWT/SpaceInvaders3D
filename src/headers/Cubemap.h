#pragma once
#include <iostream>
#include <vector>
#include <iterator>
#include "Vertex.h"
#include "Vector.h"

class Cubemap {
private:
	glm::mat4 _trans, _rot, _scale, model, view;
    GLuint vertBuffer, vertArray, elemBuffer;
    bool shouldBuffer = true;
public:
    std::vector<GLfloat> vertices {
		-1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    // std::vector<GLuint> indices {
    //     0, 1, 2,
    //     1, 2, 3,
	//
    //     3, 2, 6,
    //     2, 6, 7,
	//
    //     7, 6, 5,
    //     6, 5, 4,
	//
    //     4, 5, 1,
    //     5, 1, 0,
	//
    //     0, 3, 7,
    //     3, 7, 4,
	//
    //     1, 2, 6,
    //     2, 6, 5
	// };
	GLuint texture;

    Cubemap() {
    }
    Cubemap(bool something) {
        this->texture = loadTexture();
    }

    void BindBuffers() {
        glGenVertexArrays(1, &this->vertArray);
        glGenBuffers(1, &this->vertBuffer);
        // glGenBuffers(1, &this->elemBuffer);

        glBindVertexArray(this->vertArray);
        glBindBuffer(GL_ARRAY_BUFFER, this->vertBuffer);
        glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(GLfloat), &this->vertices[0], GL_STATIC_DRAW);

        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->elemBuffer);
        // glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
    }

	void Render(Shader shader, glm::mat4 &projection, Vector3 &viewPos, Vector3 &viewRot) {
        if (shouldBuffer) BindBuffers();

		glDepthMask(GL_FALSE);

		shader.Use();

		view = glm::translate(glm::mat4(), glm::vec3(viewPos.x, viewPos.y, viewPos.z));
		view = glm::rotate(view, glm::radians(viewRot.x), glm::vec3(1.0f, 0.0f, 0.0f));
		view = glm::rotate(view, glm::radians(viewRot.y), glm::vec3(0.0f, 1.0f, 0.0f));
		view = glm::rotate(view, glm::radians(viewRot.z), glm::vec3(0.0f, 0.0f, 1.0f));
		// view = glm::mat4(glm::mat3(view));

		glUniformMatrix4fv(glGetUniformLocation(shader.program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shader.program, "view"), 1, GL_FALSE, glm::value_ptr(glm::mat4(glm::mat3(view))));

        glBindVertexArray(this->vertArray);
        glUniform1i(glGetUniformLocation(shader.program, "skybox"), 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(0);
		glDepthMask(GL_TRUE);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

    GLuint loadTexture() {
        std::vector<const GLchar*> names {
            "assets/textures/cubemap/right.jpg",
            "assets/textures/cubemap/left.jpg",
            "assets/textures/cubemap/top.jpg",
            "assets/textures/cubemap/bottom.jpg",
            "assets/textures/cubemap/back.jpg",
            "assets/textures/cubemap/front.jpg"
        };

        GLuint textureID;
        SDL_Surface* image;

        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        for (int i = 0; i < names.size(); i++) {
            image = IMG_Load(names[i]);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, image->w, image->h, 0, GL_RGB, GL_UNSIGNED_BYTE, image->pixels);
            SDL_FreeSurface(image);
        }

        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        return textureID;
    }

    void loadCubeSide(GLuint texture, GLenum side, const char* filename) {
        SDL_Surface* image = IMG_Load(filename);
        glTexImage2D(side, 0, GL_RGB, image->w, image->h, 0, GL_RGB, GL_UNSIGNED_BYTE, image->pixels);
        SDL_FreeSurface(image);
    }
};
