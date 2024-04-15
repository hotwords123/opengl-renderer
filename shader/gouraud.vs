#version 330 core
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    bool blinn;
};

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 Color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

uniform Material material;
uniform Light light;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform bool shadows;
uniform samplerCube depthCubemap;
uniform float far;

float pointShadow(vec3 fragPos, vec3 lightPos, float bias) {
    if (!shadows) {
        return 0.0;
    }

    vec3 fragToLight = fragPos - lightPos;
    float currentDepth = length(fragToLight);
    if (currentDepth > far) {
        return 0.0;
    }

    float shadowMapDepth = far * texture(depthCubemap, fragToLight).r;
    float shadow = currentDepth > shadowMapDepth + bias ? 1.0 : 0.0;
    return shadow;
}

vec3 illuminate(Material material, Light light, vec3 fragPos, vec3 normal, vec3 viewPos, vec3 lightPos) {
    vec3 lightDir = normalize(lightPos - fragPos);
    vec3 viewDir = normalize(viewPos - fragPos);
    normal = normalize(normal) * sign(dot(normal, viewDir));

    // ambient
    vec3 ambient = material.ambient * light.ambient;

    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * material.diffuse * light.diffuse;

    // specular
    float spec = 0.0;
    if (diff > 0.0) {
        if (light.blinn) {
            vec3 halfwayDir = normalize(lightDir + viewDir);
            spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
        } else {
            vec3 reflectDir = reflect(-lightDir, normal);
            spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        }
    }
    vec3 specular = spec * material.specular * light.specular;

    // shadow
    float bias = max(4.0 * (1.0 - diff), 0.2);
    float shadow = pointShadow(fragPos, lightPos, bias);

    return ambient + (1.0 - shadow) * (diffuse + specular);
}

void main() {
    vec3 fragPos = vec3(model * vec4(aPos, 1.0));
    vec3 normal = normalMatrix * aNormal;
    Color = illuminate(material, light, fragPos, normal, viewPos, lightPos);

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
