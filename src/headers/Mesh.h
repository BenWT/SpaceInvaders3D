#pragma once
#include <iostream>
#include <vector>
#include <iterator>
#include "Vertex.h"
#include "Vector.h"

class Mesh {
private:
	glm::mat4 _trans, _rot, _scale, model, view;
    GLuint vertBuffer, vertArray, elemBuffer;
    bool shouldBuffer = true;
public:
    Vector3 position;
	Vector3 rotation;
	Vector3 scale;

    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
	GLuint texture;

    Mesh() {}
    Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, GLuint texture) {
        position.x = 0.0f;
        position.y = 0.0f;
        position.z = 0.0f;

        rotation.x = 0.0f;
        rotation.y = 0.0f;
        rotation.z = 0.0f;

        scale.x = 1.0f;
        scale.y = 1.0f;
        scale.z = 1.0f;

        this->vertices = vertices;
        this->indices = indices;
		this->texture = texture;
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

	void Render(Shader shader, glm::mat4 &projection, Vector3 &viewPos, bool doTexture) {
        if (shouldBuffer) BindBuffers();

		// Setup shader program
		shader.Use();

		// Perform Transformations
		view = glm::translate(glm::mat4(), glm::vec3(viewPos.x, viewPos.y, viewPos.z));
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(position.x, position.y, position.z));
        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(scale.x, scale.y, scale.z));

		// Set Properties
		glUniform3f(glGetUniformLocation(shader.program, "viewPos"), viewPos.x, viewPos.y, viewPos.z);
		glUniform3f(glGetUniformLocation(shader.program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
		glUniform3f(glGetUniformLocation(shader.program, "dirLight.ambient"), 0.2f, 0.2f, 0.2f);
		glUniform3f(glGetUniformLocation(shader.program, "dirLight.diffuse"), 0.5f, 0.5f, 0.5f);
		glUniform3f(glGetUniformLocation(shader.program, "dirLight.specular"), 1.0f, 1.0f, 1.0f);

		glUniformMatrix4fv(glGetUniformLocation(shader.program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(shader.program, "view"), 1, GL_FALSE, glm::value_ptr(view));

		// Do Render
		if (doTexture) glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(this->vertArray);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
		if (doTexture)  glBindTexture(GL_TEXTURE_2D, 0);
    }
};
