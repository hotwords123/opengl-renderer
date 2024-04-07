#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 Color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;
uniform vec3 objectColor;
uniform vec3 lightPos;
uniform vec3 lightColor;

void main() {
    vec3 vPos = vec3(view * model * vec4(aPos, 1.0));
    vec3 vNormal = normalMatrix * aNormal;
    vec3 vLightPos = vec3(view * vec4(lightPos, 1.0));

    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 norm = normalize(vNormal);
    vec3 lightDir = normalize(vLightPos - vPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    float specularStrength = 0.5;
    float shininess = 32;
    vec3 viewDir = normalize(-vPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;

    Color = (ambient + diffuse + specular) * objectColor;

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
