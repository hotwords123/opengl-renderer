#version 330 core
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 Color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

uniform Material material;
uniform vec3 lightPos;
uniform vec3 lightColor;

vec3 lightComponents(Material material, vec3 normal, vec3 lightDir, vec3 viewDir) {
    // ambient
    vec3 ambient = material.ambient;

    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * material.diffuse;

    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * material.specular;

    return ambient + diffuse + specular;
}

void main() {
    vec3 vPos = vec3(view * model * vec4(aPos, 1.0));
    vec3 vLightPos = vec3(view * vec4(lightPos, 1.0));

    vec3 normal = normalize(normalMatrix * aNormal);
    vec3 lightDir = normalize(vLightPos - vPos);
    vec3 viewDir = normalize(-vPos);

    Color = lightComponents(material, normal, lightDir, viewDir) * lightColor;

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
