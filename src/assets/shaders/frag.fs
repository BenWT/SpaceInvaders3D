#version 440 core

struct DirLight {
    vec3 direction, ambient, diffuse, specular;
};

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 normals;
in vec3 fragPosition;
in vec2 texCoords;

out vec4 color;

uniform vec3 viewPosition;
uniform DirLight dirLight;
uniform PointLight pointLight;
uniform sampler2D Texture;

vec3 CalculateDirLight(DirLight light, vec3 normal, vec3 viewDirection);
vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPosition, vec3 viewDir);

void main() {
    vec3 norm, viewDirection, result;

    norm = normalize(normals);
    viewDirection = normalize(viewPosition - fragPosition);

    result = CalculateDirLight(dirLight, norm, viewDirection);
    result += CalculatePointLight(pointLight, norm, fragPosition, viewDirection);

    color = vec4(result, 1.0);
}

vec3 CalculateDirLight(DirLight light, vec3 normal, vec3 viewDirection) {
    vec3 lightDirection = normalize(-dirLight.direction);

    float diff = max(dot(normal, lightDirection), 0.0);
    vec3 reflection = reflect(-lightDirection, normal);
    float spec = pow(max(dot(viewDirection, reflection), 0.0), 10.0);

    vec3 ambient = dirLight.ambient * vec3(texture(Texture, texCoords));
    vec3 diffuse = dirLight.diffuse * diff * vec3(texture(Texture, texCoords));
    vec3 specular = dirLight.specular * spec;

    return (ambient + diffuse + specular);
}

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPosition, vec3 viewDirection) {
    vec3 lightDirection = normalize(light.position - fragPosition);

    float diff = max(dot(normal, lightDirection), 0.0);

    vec3 reflectDirection = reflect(-lightDirection, normal);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), 25.0);

    float distance = length(light.position - fragPosition);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 ambient = light.ambient * vec3(texture(Texture, texCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(Texture, texCoords));
    vec3 specular = light.specular * spec * vec3(texture(Texture, texCoords));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

// vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPosition, vec3 viewDir) {
//     vec3 ambient = light.ambient * vec3(texture(Texture, texCoords));
//
//     vec3 lightDirection = normalize(light.position - fragPosition);
//     float diff = max(dot(normal, lightDirection), 0.0);
//     vec3 diffuse = light.diffuse * diff * vec3(texture(Texture, texCoords));
//
//     vec3 viewDirection = normalize(viewPosition - fragPosition);
//     vec3 reflectDirection = reflect(-lightDirection, normal);
//     float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), 50.0);
//     vec3 specular = light.specular * spec;
//
//     float distance = length(light.position - fragPosition);
//     float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
//
//     ambient *= attenuation;
//     diffuse *= attenuation;
//     specular *= attenuation;
//
//     return (ambient + diffuse + specular);
// }
