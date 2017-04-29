#version 330 core
struct DirLight {
    vec3 direction, ambient, diffuse, specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

out vec4 color;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform SpotLight spotLight;
uniform sampler2D Texture;

void main() {
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 lightDir = normalize(-dirLight.direction);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 10.0);

    vec3 ambient = dirLight.ambient * vec3(texture(Texture, TexCoord));
    vec3 diffuse = dirLight.diffuse * diff * vec3(texture(Texture, TexCoord));
    vec3 specular = dirLight.specular * spec;

    color = vec4(ambient + diffuse + specular, 1.0);
}
