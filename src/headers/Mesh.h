#pragma once
#include "Camera.h"
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
	GLuint texture;

    Mesh() {}
    Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, GLuint texture) {
		scale.Set(1.0f);

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

		shouldBuffer = false;
    }

	void Render(Shader &shader, Camera &camera, Vector3 &playerPos, glm::mat4 &projectionMat) {
        if (shouldBuffer) BindBuffers();
		shader.Use();

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(position.x, position.y, position.z));
        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(scale.x, scale.y, scale.z));

		glUniform3f(glGetUniformLocation(shader.program, "viewPos"), camera.position.x, camera.position.y, camera.position.z);
		glUniform3f(glGetUniformLocation(shader.program, "dirLight.direction"), 1.0f, -1.0f, -1.0f);
		glUniform3f(glGetUniformLocation(shader.program, "dirLight.ambient"), 0.2f, 0.2f, 0.2f);
		glUniform3f(glGetUniformLocation(shader.program, "dirLight.diffuse"), 0.5f, 0.5f, 0.5f);
		glUniform3f(glGetUniformLocation(shader.program, "dirLight.specular"), 1.0f, 1.0f, 1.0f);

		glUniform3f(glGetUniformLocation(shader.program, "pointLight.position"), playerPos.x, playerPos.y + 0.5f, playerPos.z + 0.5f);
        glUniform3f(glGetUniformLocation(shader.program, "pointLight.ambient"), 0.05f, 0.05f, 0.05f);
        glUniform3f(glGetUniformLocation(shader.program, "pointLight.diffuse"), 0.8f, 0.8f, 0.8f);
        glUniform3f(glGetUniformLocation(shader.program, "pointLight.specular"), 0.5f, 0.5f, 0.5f);
        glUniform1f(glGetUniformLocation(shader.program, "pointLight.constant"), 1.0f);
        glUniform1f(glGetUniformLocation(shader.program, "pointLight.linear"), 0.09);
        glUniform1f(glGetUniformLocation(shader.program, "pointLight.quadratic"), 0.032);

		glUniformMatrix4fv(glGetUniformLocation(shader.program, "projection"), 1, GL_FALSE, glm::value_ptr(projectionMat));
		glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(shader.program, "view"), 1, GL_FALSE, glm::value_ptr(camera.GetViewMatrix()));

		glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(this->vertArray);
        glDrawElements(GL_TRIANGLES, (GLsizei)this->indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
    }

	void RenderPlane(Shader &shader, Camera &camera, glm::mat4 &projectionMat) {
		if (shouldBuffer) BindBuffers();
		shader.Use();

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(position.x, position.y, position.z));
        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(scale.x, scale.y, scale.z));

		glUniformMatrix4fv(glGetUniformLocation(shader.program, "projection"), 1, GL_FALSE, glm::value_ptr(projectionMat));
		glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(shader.program, "view"), 1, GL_FALSE, glm::value_ptr(glm::mat4()));

		glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(this->vertArray);
        glDrawElements(GL_TRIANGLES, (GLsizei)this->indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
};
