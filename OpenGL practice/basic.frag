#version 330 core
in vec3 FragPos;
in vec3 LocalPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 u_ObjectColor;
uniform vec3 u_LightPos;
uniform float u_Alpha;
uniform float u_Emissive;
uniform float u_ClipY;

void main() {
    // Отсекаем верхнюю часть меша жидкости по текущему уровню
    if (LocalPos.y > u_ClipY) {
        discard;
    }

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(u_LightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);

    vec3 lit = (0.28 + 0.72 * diff) * u_ObjectColor;
    vec3 finalColor = mix(lit, u_ObjectColor * 1.35, u_Emissive);
    FragColor = vec4(finalColor, u_Alpha);
}