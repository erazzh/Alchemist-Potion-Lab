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
in vec3 Normal;

out vec4 FragColor;

uniform vec3 u_ViewPos;
uniform vec3 u_AmbientColor;
uniform int u_NumLights;
uniform PointLight u_Lights[MAX_LIGHTS];

uniform vec3 u_GlassTint;
uniform float u_BaseAlpha;

void main() {
    vec3 norm = normalize(Normal);
    if (!gl_FrontFacing) {
        norm = -norm;
    }

    vec3 viewDir = normalize(u_ViewPos - FragPos);

    // Аппроксимация Шлика для эффекта Френеля на гранях стекла
    float ndotv = max(dot(norm, viewDir), 0.0);
    float fresnel = 0.06 + 0.94 * pow(1.0 - ndotv, 3.0);

    vec3 rimLight = u_AmbientColor * 1.6 * u_GlassTint;
    vec3 specularTotal = vec3(0.0);

    int count = min(u_NumLights, MAX_LIGHTS);
    for (int i = 0; i < count; ++i) {
        if (u_Lights[i].intensity <= 0.001) continue;

        vec3 lightVec = u_Lights[i].position - FragPos;
        float dist = length(lightVec);
        vec3 lightDir = lightVec / max(dist, 0.0001);

        float att = 1.0 / (u_Lights[i].constant + u_Lights[i].linear * dist + u_Lights[i].quadratic * dist * dist);

        // Острый стеклянный блик Blinn-Phong
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(norm, halfwayDir), 0.0), 160.0);
        specularTotal += u_Lights[i].color * spec * 1.9 * u_Lights[i].intensity * att;

        // Преломлённое подсвечивание краёв колбы ближайшими источниками света (свечи + зелье)
        float wrapDiff = max(dot(norm, lightDir) * 0.5 + 0.5, 0.0);
        rimLight += u_Lights[i].color * wrapDiff * 0.45 * u_Lights[i].intensity * att;
    }

    vec3 color = u_GlassTint * (0.18 + 0.85 * fresnel) + rimLight * (0.35 + 0.95 * fresnel) + specularTotal;
    color = color / (color + vec3(0.75)) * 1.45;

    // В центре стекло почти невидимо, а по краям и в зонах бликов становится плотнее
    float specBoost = clamp(length(specularTotal) * 0.55, 0.0, 0.55);
    float alpha = clamp(u_BaseAlpha + fresnel * 0.58 + specBoost, 0.08, 0.88);

    FragColor = vec4(color, alpha);
}