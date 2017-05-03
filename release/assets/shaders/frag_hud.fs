#version 440 core

out vec4 color;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

uniform sampler2D Texture;

void main() {
    color = texture(Texture, TexCoord);
}
