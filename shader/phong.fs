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

in vec3 FragPos;
in vec3 Normal;
in vec3 LightPos;

out vec4 FragColor;

uniform Material material;
uniform Light light;

vec3 illuminate(Material material, Light light, vec3 fragPos, vec3 normal, vec3 lightPos) {
    vec3 lightDir = normalize(lightPos - fragPos);
    vec3 viewDir = normalize(-fragPos);
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

    return ambient + diffuse + specular;
}

void main() {
    vec3 result = illuminate(material, light, FragPos, Normal, LightPos);
    FragColor = vec4(result, 1.0);
}
