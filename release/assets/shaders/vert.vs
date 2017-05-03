#version 440 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

out vec3 normals;
out vec3 fragPosition;
out vec2 texCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
    gl_Position = projection * view *  model * vec4(position, 1.0f);
    normals = mat3(transpose(inverse(model))) * normal;
    fragPosition = vec3(model * vec4(position, 1.0f));
    texCoords = vec2(texCoord.x, texCoord.y);
}
