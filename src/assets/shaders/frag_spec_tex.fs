#version 330 core
out vec4 color;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform sampler2D Texture;

void main()
{
    // Ambient
    float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * lightColor * vec3(texture(Texture, TexCoord));

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * vec3(texture(Texture, TexCoord));

    // Specular
    float specularStrength = 0.5f;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * vec3(0.2f, 0.2f, 0.2f);
    color = vec4(result, 1.0f);
}