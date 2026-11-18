#version 330 core
#define MAX_LIGHTS 8

struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
    float constant;
    float linear;
    float quadratic;
};

in vec3 FragPos;
in vec3 LocalPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 u_ViewPos;
uniform vec3 u_AmbientColor;
uniform int u_NumLights;
uniform PointLight u_Lights[MAX_LIGHTS];

uniform sampler2D u_DiffuseMap;
uniform vec3 u_ObjectColor;
uniform float u_Shininess;
uniform float u_SpecularStrength;
uniform float u_Alpha;
uniform float u_Emissive;
uniform float u_ClipY;

vec3 calcPointLightBlinnPhong(PointLight light, vec3 norm, vec3 fragPos, vec3 viewDir, vec3 albedo) {
    if (light.intensity <= 0.001) {
        return vec3(0.0);
    }

    vec3 lightVec = light.position - fragPos;
    float dist = length(lightVec);
    vec3 lightDir = lightVec / max(dist, 0.0001);

    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * dist * dist);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.color * albedo;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), u_Shininess);
    vec3 specular = spec * u_SpecularStrength * light.color;

    return (diffuse + specular) * light.intensity * attenuation;
}

void main() {
    if (LocalPos.y > u_ClipY) {
        discard;
    }

    vec3 norm = normalize(Normal);
    if (!gl_FrontFacing) {
        norm = -norm;
    }

    vec3 viewDir = normalize(u_ViewPos - FragPos);
    vec3 albedo = texture(u_DiffuseMap, TexCoords).rgb * u_ObjectColor;

    vec3 result = u_AmbientColor * albedo;

    int count = min(u_NumLights, MAX_LIGHTS);
    for (int i = 0; i < count; ++i) {
        result += calcPointLightBlinnPhong(u_Lights[i], norm, FragPos, viewDir, albedo);
    }

    result += albedo * u_Emissive;
    result = result / (result + vec3(0.65)) * 1.45;

    FragColor = vec4(result, u_Alpha);
}