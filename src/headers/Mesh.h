#pragma once
#include <iostream>
#include <vector>
#include <iterator>
#include "Vertex.h"
#include "Vector.h"

class Mesh {
private:
	glm::mat4 _trans, _rot, _scale, model;
    GLuint vertBuffer, vertArray, elemBuffer;
    bool shouldBuffer = true;
public:
    Vector3 position;
	Vector3 rotation;
	Vector3 scale;

    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    Mesh() {}
    Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices) {
        position.x = 0.0f;
        position.y = 0.0f;
        position.z = -5.0f;

        rotation.x = 0.0f;
        rotation.y = 0.0f;
        rotation.z = 0.0f;

        scale.x = 0.2f;
        scale.y = 0.2f;
        scale.z = 0.2f;

        this->vertices = vertices;
        this->indices = indices;
    }

    void BindBuffers() {
        glGenVertexArrays(1, &this->vertArray);
        glGenBuffers(1, &this->vertBuffer);
        glGenBuffers(1, &this->elemBuffer);

        glBindVertexArray(this->vertArray);
        glBindBuffer(GL_ARRAY_BUFFER, this->vertBuffer);
        glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->elemBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texCoords));

        glBindVertexArray(0);
    }

	void Render(Shader shader, glm::mat4 &projection, glm::mat4 &view) {
        if (shouldBuffer) BindBuffers();
		shader.Use();

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(position.x, position.y, position.z));
        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(scale.x, scale.y, scale.z));

		GLint lightColorLocation = glGetUniformLocation(shader.program, "lightColor");
		GLint lightPosLocation = glGetUniformLocation(shader.program, "lightPos");
		GLint viewPosLocation = glGetUniformLocation(shader.program, "viewPos");
		glUniform3f(lightColorLocation, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightPosLocation, -10.0f, 0.0f, 10.0f);
		glUniform3f(viewPosLocation, 0.0f, 0.0f, 0.0f);

		GLint projectionLocation = glGetUniformLocation(shader.program, "projection");
		GLint modelLocation = glGetUniformLocation(shader.program, "model");
		GLint viewLocation = glGetUniformLocation(shader.program, "view");
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));

        glBindVertexArray(this->vertArray);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0); // GL_TRIANGLES | GL_LINES
        glBindVertexArray(0);
    }
};
