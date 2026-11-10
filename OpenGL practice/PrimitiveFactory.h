#pragma once
#include "Mesh.h"
#include <glm/gtc/constants.hpp>
#include <cmath>
#include <vector>

class PrimitiveFactory {
public:
    // 1.  уб 1x1x1 с центром в (0, 0, 0) Ч стол, книги, стены, полки, бруски
    static Mesh CreateCube() {
        std::vector<Vertex> vertices = {
            // Front (+Z)
            {{-0.5f,-0.5f, 0.5f}, { 0, 0, 1}, {0,0}}, {{ 0.5f,-0.5f, 0.5f}, { 0, 0, 1}, {1,0}},
            {{ 0.5f, 0.5f, 0.5f}, { 0, 0, 1}, {1,1}}, {{-0.5f, 0.5f, 0.5f}, { 0, 0, 1}, {0,1}},
            // Back (-Z)
            {{ 0.5f,-0.5f,-0.5f}, { 0, 0,-1}, {0,0}}, {{-0.5f,-0.5f,-0.5f}, { 0, 0,-1}, {1,0}},
            {{-0.5f, 0.5f,-0.5f}, { 0, 0,-1}, {1,1}}, {{ 0.5f, 0.5f,-0.5f}, { 0, 0,-1}, {0,1}},
            // Left (-X)
            {{-0.5f,-0.5f,-0.5f}, {-1, 0, 0}, {0,0}}, {{-0.5f,-0.5f, 0.5f}, {-1, 0, 0}, {1,0}},
            {{-0.5f, 0.5f, 0.5f}, {-1, 0, 0}, {1,1}}, {{-0.5f, 0.5f,-0.5f}, {-1, 0, 0}, {0,1}},
            // Right (+X)
            {{ 0.5f,-0.5f, 0.5f}, { 1, 0, 0}, {0,0}}, {{ 0.5f,-0.5f,-0.5f}, { 1, 0, 0}, {1,0}},
            {{ 0.5f, 0.5f,-0.5f}, { 1, 0, 0}, {1,1}}, {{ 0.5f, 0.5f, 0.5f}, { 1, 0, 0}, {0,1}},
            // Top (+Y)
            {{-0.5f, 0.5f, 0.5f}, { 0, 1, 0}, {0,0}}, {{ 0.5f, 0.5f, 0.5f}, { 0, 1, 0}, {1,0}},
            {{ 0.5f, 0.5f,-0.5f}, { 0, 1, 0}, {1,1}}, {{-0.5f, 0.5f,-0.5f}, { 0, 1, 0}, {0,1}},
            // Bottom (-Y)
            {{-0.5f,-0.5f,-0.5f}, { 0,-1, 0}, {0,0}}, {{ 0.5f,-0.5f,-0.5f}, { 0,-1, 0}, {1,0}},
            {{ 0.5f,-0.5f, 0.5f}, { 0,-1, 0}, {1,1}}, {{-0.5f,-0.5f, 0.5f}, { 0,-1, 0}, {0,1}}
        };

        std::vector<unsigned int> indices;
        for (unsigned int face = 0; face < 6; ++face) {
            unsigned int base = face * 4;
            indices.insert(indices.end(), { base, base + 1, base + 2, base, base + 2, base + 3 });
        }
        return Mesh(vertices, indices);
    }

    // 2. ÷илиндр / усечЄнный конус (y от 0 до height) Ч свечи, пробки, ножки треноги
    static Mesh CreateCylinder(float radiusBottom, float radiusTop, float height,
        int sectors = 32, bool capTop = true, bool capBottom = true) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        const float twoPi = 2.0f * glm::pi<float>();

        float slope = (radiusBottom - radiusTop) / height;
        for (int i = 0; i <= sectors; ++i) {
            float u = static_cast<float>(i) / sectors;
            float angle = u * twoPi;
            float ca = std::cos(angle), sa = std::sin(angle);

            glm::vec3 normal = glm::normalize(glm::vec3(ca, slope, sa));
            vertices.push_back({ {radiusBottom * ca, 0.0f,   radiusBottom * sa}, normal, {u, 0.0f} });
            vertices.push_back({ {radiusTop * ca,    height, radiusTop * sa},    normal, {u, 1.0f} });
        }

        for (int i = 0; i < sectors; ++i) {
            unsigned int b0 = i * 2, t0 = b0 + 1, b1 = b0 + 2, t1 = b0 + 3;
            indices.insert(indices.end(), { b0, t0, b1, t0, t1, b1 });
        }

        if (capTop && radiusTop > 0.0f) {
            unsigned int centerIdx = static_cast<unsigned int>(vertices.size());
            vertices.push_back({ {0.0f, height, 0.0f}, {0, 1, 0}, {0.5f, 0.5f} });
            for (int i = 0; i <= sectors; ++i) {
                float angle = (static_cast<float>(i) / sectors) * twoPi;
                float ca = std::cos(angle), sa = std::sin(angle);
                vertices.push_back({ {radiusTop * ca, height, radiusTop * sa}, {0, 1, 0}, {0.5f + 0.5f * ca, 0.5f + 0.5f * sa} });
            }
            for (int i = 0; i < sectors; ++i) {
                indices.insert(indices.end(), { centerIdx, centerIdx + 1 + i + 1, centerIdx + 1 + i });
            }
        }

        if (capBottom && radiusBottom > 0.0f) {
            unsigned int centerIdx = static_cast<unsigned int>(vertices.size());
            vertices.push_back({ {0.0f, 0.0f, 0.0f}, {0, -1, 0}, {0.5f, 0.5f} });
            for (int i = 0; i <= sectors; ++i) {
                float angle = (static_cast<float>(i) / sectors) * twoPi;
                float ca = std::cos(angle), sa = std::sin(angle);
                vertices.push_back({ {radiusBottom * ca, 0.0f, radiusBottom * sa}, {0, -1, 0}, {0.5f + 0.5f * ca, 0.5f + 0.5f * sa} });
            }
            for (int i = 0; i < sectors; ++i) {
                indices.insert(indices.end(), { centerIdx, centerIdx + 1 + i, centerIdx + 1 + i + 1 });
            }
        }

        return Mesh(vertices, indices);
    }

    // 3. “ело вращени€ по 2D-профилю (x = радиус, y = высота) Ч пузатые колбы и жидкость
    static Mesh CreateLatheMesh(const std::vector<glm::vec2>& profile, int sectors = 40) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        const float twoPi = 2.0f * glm::pi<float>();
        const size_t n = profile.size();

        for (size_t i = 0; i < n; ++i) {
            float r = profile[i].x;
            float y = profile[i].y;
            float v = static_cast<float>(i) / static_cast<float>(n - 1);

            // ¬ычисл€ем гладкую касательную профил€ дл€ корректных нормалей
            glm::vec2 prev = (i > 0) ? profile[i - 1] : profile[i];
            glm::vec2 next = (i + 1 < n) ? profile[i + 1] : profile[i];
            float dr = next.x - prev.x;
            float dy = next.y - prev.y;

            for (int j = 0; j <= sectors; ++j) {
                float u = static_cast<float>(j) / sectors;
                float theta = u * twoPi;
                float ca = std::cos(theta);
                float sa = std::sin(theta);

                glm::vec3 pos(r * ca, y, r * sa);
                glm::vec3 norm(dy * ca, -dr, dy * sa);
                if (glm::length(norm) < 0.0001f) norm = glm::vec3(0.0f, -1.0f, 0.0f);
                else norm = glm::normalize(norm);

                vertices.push_back({ pos, norm, {u, v} });
            }
        }

        const unsigned int ringStride = static_cast<unsigned int>(sectors + 1);
        for (size_t i = 0; i + 1 < n; ++i) {
            for (int j = 0; j < sectors; ++j) {
                unsigned int a = static_cast<unsigned int>(i) * ringStride + j;
                unsigned int b = a + 1;
                unsigned int c = a + ringStride;
                unsigned int d = c + 1;
                indices.insert(indices.end(), { a, c, b, c, d, b });
            }
        }
        return Mesh(vertices, indices);
    }

    // 4.  руглый диск радиуса 1 в плоскости XZ Ч верхн€€ поверхность (мениск) жидкости
    static Mesh CreateDisk(float radius = 1.0f, int sectors = 40) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        const float twoPi = 2.0f * glm::pi<float>();

        vertices.push_back({ {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.5f, 0.5f} });
        for (int i = 0; i <= sectors; ++i) {
            float angle = (static_cast<float>(i) / sectors) * twoPi;
            float ca = std::cos(angle), sa = std::sin(angle);
            vertices.push_back({ {radius * ca, 0.0f, radius * sa}, {0.0f, 1.0f, 0.0f}, {0.5f + 0.5f * ca, 0.5f + 0.5f * sa} });
        }
        for (int i = 0; i < sectors; ++i) {
            indices.insert(indices.end(), { 0, static_cast<unsigned int>(i + 2), static_cast<unsigned int>(i + 1) });
        }
        return Mesh(vertices, indices);
    }

    // 5. UV-сфера Ч €зычки пламени свечей, будущие пузырьки
    static Mesh CreateSphere(float radius = 0.5f, int stacks = 20, int sectors = 28) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        const float pi = glm::pi<float>();

        for (int i = 0; i <= stacks; ++i) {
            float v = static_cast<float>(i) / stacks;
            float phi = pi * 0.5f - v * pi;
            float xy = std::cos(phi);
            float y = std::sin(phi);

            for (int j = 0; j <= sectors; ++j) {
                float u = static_cast<float>(j) / sectors;
                float theta = u * 2.0f * pi;
                float x = xy * std::cos(theta);
                float z = xy * std::sin(theta);

                glm::vec3 normal(x, y, z);
                vertices.push_back({ normal * radius, normal, {u, 1.0f - v} });
            }
        }

        for (int i = 0; i < stacks; ++i) {
            unsigned int k1 = i * (sectors + 1);
            unsigned int k2 = k1 + sectors + 1;
            for (int j = 0; j < sectors; ++j, ++k1, ++k2) {
                if (i != 0)          indices.insert(indices.end(), { k1, k1 + 1, k2 });
                if (i != stacks - 1) indices.insert(indices.end(), { k1 + 1, k2 + 1, k2 });
            }
        }
        return Mesh(vertices, indices);
    }

    // 6. ѕлоскость XZ Ч магический круг на столе, пол
    static Mesh CreatePlane(float width = 1.0f, float depth = 1.0f) {
        float hw = width * 0.5f, hd = depth * 0.5f;
        std::vector<Vertex> vertices = {
            {{-hw, 0.0f,  hd}, {0, 1, 0}, {0, 0}},
            {{ hw, 0.0f,  hd}, {0, 1, 0}, {1, 0}},
            {{ hw, 0.0f, -hd}, {0, 1, 0}, {1, 1}},
            {{-hw, 0.0f, -hd}, {0, 1, 0}, {0, 1}}
        };
        std::vector<unsigned int> indices = { 0, 1, 2, 0, 2, 3 };
        return Mesh(vertices, indices);
    }
};