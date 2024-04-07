#version 330 core
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

in vec3 FragPos;
in vec3 Normal;
in vec3 LightPos;

out vec4 FragColor;

uniform Material material;
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
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    vec3 viewDir = normalize(-FragPos);

    vec3 result = lightComponents(material, norm, lightDir, viewDir) * lightColor;
    FragColor = vec4(result, 1.0);
}
