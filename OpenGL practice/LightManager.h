#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <cmath>
#include "Shader.h"

struct PointLight {
    glm::vec3 Position;
    glm::vec3 Color;
    float Intensity;
    float Constant;
    float Linear;
    float Quadratic;
};

class LightManager {
public:
    std::vector<PointLight> Lights;
    glm::vec3 AmbientColor = glm::vec3(0.08f, 0.08f, 0.11f);

    // Обновляем 3 мерцающие свечи + 1 динамический свет внутри главной колбы
    void Update(float time,
        bool candlesLit,
        const std::vector<glm::vec3>& candleFlamePositions,
        const glm::vec3& potionPos,
        const glm::vec3& potionColor,
        float liquidLevel) {
        Lights.clear();

        // При выключенных свечах делаем фоновое освещение тёмным для контраста "Wow-момента"
        AmbientColor = candlesLit ? glm::vec3(0.08f, 0.07f, 0.09f)
            : glm::vec3(0.025f, 0.025f, 0.04f);

        // 1. Источники света от свечей (тёплый огонь с гармоническим мерцанием и лёгким колебанием)
        float baseIntensities[3] = { 0.95f, 0.70f, 0.80f };
        for (size_t i = 0; i < candleFlamePositions.size() && i < 3; ++i) {
            float phase = static_cast<float>(i) * 2.3f;
            float flicker = 1.0f
                + 0.11f * std::sin(time * 7.3f + phase)
                + 0.07f * std::sin(time * 17.1f + phase) * std::cos(time * 3.7f);

            // Лёгкое смещение источника света имитирует движение язычка пламени на сквозняке
            glm::vec3 sway(
                0.012f * std::sin(time * 5.2f + phase),
                0.008f * std::cos(time * 8.1f + phase),
                0.012f * std::cos(time * 4.7f + phase)
            );

            PointLight candle;
            candle.Position = candleFlamePositions[i] + sway;
            candle.Color = glm::vec3(1.0f, 0.58f, 0.20f);
            candle.Intensity = candlesLit ? (baseIntensities[i] * flicker) : 0.0f;
            candle.Constant = 1.0f;
            candle.Linear = 0.22f;
            candle.Quadratic = 0.20f;
            Lights.push_back(candle);
        }

        // 2. Динамический источник света от зелья в главной колбе
        float magicPulse = 1.0f + 0.08f * std::sin(time * 3.2f);
        float darkBoost = candlesLit ? 1.0f : 1.35f; // В темноте магический свет ощущается ярче

        PointLight potion;
        potion.Position = potionPos;
        potion.Color = potionColor;
        potion.Intensity = (liquidLevel > 0.01f)
            ? ((0.35f + 1.55f * liquidLevel) * magicPulse * darkBoost)
            : 0.0f;
        potion.Constant = 1.0f;
        potion.Linear = 0.14f;
        potion.Quadratic = 0.08f;
        Lights.push_back(potion);
    }

    void ApplyToShader(const Shader& shader) const {
        shader.setVec3("u_AmbientColor", AmbientColor);
        shader.setInt("u_NumLights", static_cast<int>(Lights.size()));

        for (size_t i = 0; i < Lights.size(); ++i) {
            std::string prefix = "u_Lights[" + std::to_string(i) + "].";
            shader.setVec3(prefix + "position", Lights[i].Position);
            shader.setVec3(prefix + "color", Lights[i].Color);
            shader.setFloat(prefix + "intensity", Lights[i].Intensity);
            shader.setFloat(prefix + "constant", Lights[i].Constant);
            shader.setFloat(prefix + "linear", Lights[i].Linear);
            shader.setFloat(prefix + "quadratic", Lights[i].Quadratic);
        }
    }
};