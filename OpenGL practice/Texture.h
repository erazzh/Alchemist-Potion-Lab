#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <cmath>
#include <algorithm>

class Texture {
public:
    unsigned int ID = 0;

    Texture() = default;
    ~Texture() { cleanup(); }

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    Texture(Texture&& other) noexcept : ID(other.ID) { other.ID = 0; }
    Texture& operator=(Texture&& other) noexcept {
        if (this != &other) {
            cleanup();
            ID = other.ID;
            other.ID = 0;
        }
        return *this;
    }

    void Bind(unsigned int slot = 0) const {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, ID);
    }

    // 1. Базовая белая 1x1 текстура (для объектов без рисунка: воск, металл, жидкость)
    static Texture CreateWhite() {
        std::vector<unsigned char> data = { 255, 255, 255 };
        return uploadRGB(1, 1, data);
    }

    // 2. Текстура старых деревянных досок (столешница, ножки, полка, штатив)
    static Texture CreateWoodPlanks(int size = 512) {
        std::vector<unsigned char> data(size * size * 3);
        for (int y = 0; y < size; ++y) {
            for (int x = 0; x < size; ++x) {
                float u = static_cast<float>(x) / size;
                float v = static_cast<float>(y) / size;

                int plankIdx = static_cast<int>(v * 6.0f);
                float localV = std::fmod(v * 6.0f, 1.0f);
                float seam = (localV < 0.035f || localV > 0.965f) ? 0.35f : 1.0f;

                // Волокна древесины с мягким шумом
                float n = valueNoise(u * 14.0f + plankIdx * 3.7f, v * 48.0f);
                float grain = 0.5f + 0.5f * std::sin((v * 55.0f + n * 5.0f) * 3.14159f);
                float plankTone = 0.88f + 0.24f * hash2(static_cast<float>(plankIdx), 11.0f);

                float shade = (0.68f + 0.32f * grain) * plankTone * seam;
                writePixel(data, x, y, size,
                    glm::vec3(0.46f, 0.29f, 0.16f) * shade);
            }
        }
        return uploadRGB(size, size, data);
    }

    // 3. Текстура средневековой каменной кладки (стены и пол лаборатории)
    static Texture CreateStoneBricks(int size = 512) {
        std::vector<unsigned char> data(size * size * 3);
        const float rows = 8.0f;
        const float cols = 4.0f;

        for (int y = 0; y < size; ++y) {
            for (int x = 0; x < size; ++x) {
                float u = static_cast<float>(x) / size;
                float v = static_cast<float>(y) / size;

                int row = static_cast<int>(v * rows);
                float offsetU = (row % 2 == 1) ? 0.5f / cols : 0.0f;
                float tiledU = std::fmod((u + offsetU) * cols, 1.0f);
                float tiledV = std::fmod(v * rows, 1.0f);
                int col = static_cast<int>(std::floor((u + offsetU) * cols));

                bool isMortar = (tiledU < 0.045f || tiledU > 0.955f || tiledV < 0.07f || tiledV > 0.93f);
                float grain = valueNoise(u * 32.0f, v * 32.0f) * 0.5f + valueNoise(u * 64.0f, v * 64.0f) * 0.5f;

                glm::vec3 color;
                if (isMortar) {
                    color = glm::vec3(0.14f, 0.14f, 0.16f) * (0.85f + 0.3f * grain);
                }
                else {
                    float blockVar = 0.78f + 0.36f * hash2(static_cast<float>(col), static_cast<float>(row));
                    color = glm::vec3(0.38f, 0.37f, 0.41f) * blockVar * (0.78f + 0.34f * grain);
                }
                writePixel(data, x, y, size, color);
            }
        }
        return uploadRGB(size, size, data);
    }

    // 4. Текстура потёртой кожи с золотой рамкой (переплёты алхимических книг)
    static Texture CreateLeatherBook(int size = 256) {
        std::vector<unsigned char> data(size * size * 3);
        for (int y = 0; y < size; ++y) {
            for (int x = 0; x < size; ++x) {
                float u = static_cast<float>(x) / size;
                float v = static_cast<float>(y) / size;

                float n = valueNoise(u * 26.0f, v * 26.0f) * 0.6f + valueNoise(u * 58.0f, v * 58.0f) * 0.4f;
                float border = std::min(std::min(u, 1.0f - u), std::min(v, 1.0f - v));

                glm::vec3 col = glm::vec3(0.85f + 0.25f * n);
                // Тонкая тисненая рамка вдоль краёв обложки
                if (border > 0.07f && border < 0.105f) {
                    col = glm::vec3(1.15f, 1.02f, 0.65f);
                }
                writePixel(data, x, y, size, col);
            }
        }
        return uploadRGB(size, size, data);
    }

    // 5. Текстура пергамента со строчками записей (раскрытая книга справа)
    static Texture CreateParchment(int size = 256) {
        std::vector<unsigned char> data(size * size * 3);
        for (int y = 0; y < size; ++y) {
            for (int x = 0; x < size; ++x) {
                float u = static_cast<float>(x) / size;
                float v = static_cast<float>(y) / size;

                float n = valueNoise(u * 16.0f, v * 16.0f);
                glm::vec3 paper = glm::vec3(0.90f, 0.83f, 0.66f) * (0.88f + 0.16f * n);

                // Центральный сгиб разворота
                if (std::abs(u - 0.5f) < 0.025f) {
                    paper *= 0.60f;
                }
                else if (u > 0.08f && u < 0.92f && std::abs(u - 0.5f) > 0.06f && v > 0.12f && v < 0.88f) {
                    float lineWave = std::sin(v * 95.0f);
                    float inkNoise = valueNoise(u * 40.0f, v * 20.0f);
                    if (lineWave > 0.82f && inkNoise > 0.35f) {
                        paper *= 0.48f; // Имитация рукописных формул
                    }
                }
                writePixel(data, x, y, size, paper);
            }
        }
        return uploadRGB(size, size, data);
    }

private:
    static float hash2(float x, float y) {
        float h = std::sin(x * 127.1f + y * 311.7f) * 43758.5453f;
        return h - std::floor(h);
    }

    static float valueNoise(float x, float y) {
        float ix = std::floor(x), iy = std::floor(y);
        float fx = x - ix, fy = y - iy;
        fx = fx * fx * (3.0f - 2.0f * fx);
        fy = fy * fy * (3.0f - 2.0f * fy);

        float a = hash2(ix, iy);
        float b = hash2(ix + 1.0f, iy);
        float c = hash2(ix, iy + 1.0f);
        float d = hash2(ix + 1.0f, iy + 1.0f);
        return glm::mix(glm::mix(a, b, fx), glm::mix(c, d, fx), fy);
    }

    static void writePixel(std::vector<unsigned char>& data, int x, int y, int size, const glm::vec3& c) {
        int idx = (y * size + x) * 3;
        data[idx + 0] = static_cast<unsigned char>(glm::clamp(c.r, 0.0f, 1.0f) * 255.0f);
        data[idx + 1] = static_cast<unsigned char>(glm::clamp(c.g, 0.0f, 1.0f) * 255.0f);
        data[idx + 2] = static_cast<unsigned char>(glm::clamp(c.b, 0.0f, 1.0f) * 255.0f);
    }

    static Texture uploadRGB(int width, int height, const std::vector<unsigned char>& data) {
        Texture tex;
        glGenTextures(1, &tex.ID);
        glBindTexture(GL_TEXTURE_2D, tex.ID);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());
        glGenerateMipmap(GL_TEXTURE_2D); // Обязательный мипмаппинг (R5)

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);
        return tex;
    }

    void cleanup() {
        if (ID != 0) glDeleteTextures(1, &ID);
    }
};