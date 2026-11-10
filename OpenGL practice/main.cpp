#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "Mesh.h"
#include "PrimitiveFactory.h"
#include "Camera.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <cmath>

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

Camera camera({ 0.0f, 1.55f, 3.15f }, -90.0f, -21.0f);
float lastX = SCR_WIDTH * 0.5f, lastY = SCR_HEIGHT * 0.5f;
bool firstMouse = true;
bool mouseCaptured = true;

// Состояние зелья (Core темы 15)
float liquidLevel = 0.45f;
float targetLiquidLevel = 0.45f;
glm::vec3 currentPotionColor(0.55f, 0.20f, 0.90f);
glm::vec3 targetPotionColor(0.55f, 0.20f, 0.90f);
bool stirrerActive = true;
bool candlesLit = true;

void addIngredient(const glm::vec3& ingColor) {
    float volumeAdd = 0.16f;
    float newLevel = std::min(1.0f, targetLiquidLevel + volumeAdd);
    float weight = volumeAdd / std::max(0.01f, newLevel);
    targetPotionColor = glm::mix(targetPotionColor, ingColor, weight);
    targetLiquidLevel = newLevel;
}

// Вычисляем точный радиус профиля колбы на заданной высоте y (для мениска жидкости)
float sampleProfileRadius(const std::vector<glm::vec2>& profile, float y) {
    if (y <= profile.front().y) return profile.front().x;
    for (size_t i = 0; i + 1 < profile.size(); ++i) {
        if (y >= profile[i].y && y <= profile[i + 1].y) {
            float span = std::max(0.0001f, profile[i + 1].y - profile[i].y);
            float t = (y - profile[i].y) / span;
            return glm::mix(profile[i].x, profile[i + 1].x, t);
        }
    }
    return profile.back().x;
}

void framebuffer_size_callback(GLFWwindow*, int w, int h) { glViewport(0, 0, w, h); }

void mouse_callback(GLFWwindow*, double xposIn, double yposIn) {
    if (!mouseCaptured) return;
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (firstMouse) { lastX = xpos; lastY = ypos; firstMouse = false; }
    camera.ProcessMouse(xpos - lastX, lastY - ypos);
    lastX = xpos; lastY = ypos;
}

void key_callback(GLFWwindow* window, int key, int, int action, int) {
    if (action != GLFW_PRESS) return;
    if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, true);
    if (key == GLFW_KEY_TAB) {
        mouseCaptured = !mouseCaptured;
        firstMouse = true;
        glfwSetInputMode(window, GLFW_CURSOR, mouseCaptured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }
    if (key == GLFW_KEY_1) addIngredient({ 0.95f, 0.15f, 0.20f }); // Красный реагент
    if (key == GLFW_KEY_2) addIngredient({ 0.15f, 0.35f, 0.98f }); // Синий реагент
    if (key == GLFW_KEY_3) addIngredient({ 0.15f, 0.95f, 0.35f }); // Изумрудный реагент
    if (key == GLFW_KEY_R) { targetLiquidLevel = 0.0f; }           // Слить колбу
    if (key == GLFW_KEY_M) { stirrerActive = !stirrerActive; }     // Вкл/выкл мешалку
    if (key == GLFW_KEY_C) { candlesLit = !candlesLit; }           // Зажечь/потушить свечи
}

void processMovement(GLFWwindow* window, float dt) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.ProcessKeyboard(CameraMove::FORWARD, dt);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.ProcessKeyboard(CameraMove::BACKWARD, dt);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.ProcessKeyboard(CameraMove::LEFT, dt);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.ProcessKeyboard(CameraMove::RIGHT, dt);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) camera.ProcessKeyboard(CameraMove::UP, dt);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) camera.ProcessKeyboard(CameraMove::DOWN, dt);
}

void drawMesh(const Shader& shader, const Mesh& mesh, const glm::mat4& model,
    const glm::vec3& color, float alpha = 1.0f, float emissive = 0.0f, float clipY = 999.0f) {
    shader.setMat4("u_Model", model);
    shader.setVec3("u_ObjectColor", color);
    shader.setFloat("u_Alpha", alpha);
    shader.setFloat("u_Emissive", emissive);
    shader.setFloat("u_ClipY", clipY);
    mesh.Draw();
}

int main() {
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Alchemist's Potion Lab", nullptr, nullptr);
    if (!window) { glfwTerminate(); return -1; }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Shader shader("basic.vert", "basic.frag");

    // 1. Базовые примитивы
    Mesh cube = PrimitiveFactory::CreateCube();
    Mesh cylinder = PrimitiveFactory::CreateCylinder(0.5f, 0.5f, 1.0f, 32);
    Mesh sphere = PrimitiveFactory::CreateSphere(0.5f, 20, 28);
    Mesh disk = PrimitiveFactory::CreateDisk(1.0f, 40);

    // 2. Профиль пузатой главной алхимической колбы (x = радиус, y = высота)
    std::vector<glm::vec2> mainFlaskProfile = {
        {0.00f, 0.00f}, {0.18f, 0.00f}, {0.32f, 0.05f}, {0.42f, 0.16f},
        {0.46f, 0.30f}, {0.44f, 0.45f}, {0.36f, 0.58f}, {0.23f, 0.67f},
        {0.15f, 0.74f}, {0.14f, 0.96f}, {0.19f, 1.02f}, {0.17f, 1.05f}
    };
    std::vector<glm::vec2> mainLiquidProfile;
    for (size_t i = 0; i < 9; ++i) { // Повторяет внутреннюю форму колбы с отступом 6% (защита от z-fighting)
        mainLiquidProfile.push_back({ mainFlaskProfile[i].x * 0.93f, mainFlaskProfile[i].y + 0.015f });
    }
    Mesh mainFlaskGlass = PrimitiveFactory::CreateLatheMesh(mainFlaskProfile, 44);
    Mesh mainFlaskLiquid = PrimitiveFactory::CreateLatheMesh(mainLiquidProfile, 44);

    // 3. Профиль маленьких колб для реагентов (стекло + внутренняя жидкость)
    std::vector<glm::vec2> smallVialProfile = {
        {0.00f, 0.00f}, {0.06f, 0.00f}, {0.11f, 0.04f}, {0.125f, 0.12f},
        {0.11f, 0.21f}, {0.055f, 0.27f}, {0.048f, 0.39f}, {0.065f, 0.42f}
    };
    std::vector<glm::vec2> smallLiquidProfile;
    for (size_t i = 0; i < 6; ++i) {
        smallLiquidProfile.push_back({ smallVialProfile[i].x * 0.88f, smallVialProfile[i].y + 0.012f });
    }
    Mesh smallVialGlass = PrimitiveFactory::CreateLatheMesh(smallVialProfile, 28);
    Mesh smallVialLiquid = PrimitiveFactory::CreateLatheMesh(smallLiquidProfile, 28);

    // 4. Железное кольцо треноги
    std::vector<glm::vec2> ringProfile = {
        {0.31f, 0.00f}, {0.36f, 0.00f}, {0.36f, 0.045f}, {0.31f, 0.045f}, {0.31f, 0.00f}
    };
    Mesh tripodRing = PrimitiveFactory::CreateLatheMesh(ringProfile, 36);

    float lastFrame = 0.0f, fpsTimer = 0.0f, stirAngle = 0.0f;
    int frameCount = 0;

    while (!glfwWindowShouldClose(window)) {
        float now = static_cast<float>(glfwGetTime());
        float dt = now - lastFrame;
        lastFrame = now;

        // Счётчик FPS в заголовке (R7)
        frameCount++;
        fpsTimer += dt;
        if (fpsTimer >= 0.5f) {
            std::ostringstream title;
            title << "Alchemist's Potion Lab | FPS: " << std::fixed << std::setprecision(1) << (frameCount / fpsTimer);
            glfwSetWindowTitle(window, title.str().c_str());
            frameCount = 0; fpsTimer = 0.0f;
        }

        processMovement(window, dt);

        liquidLevel = glm::mix(liquidLevel, targetLiquidLevel, std::min(1.0f, dt * 4.0f));
        currentPotionColor = glm::mix(currentPotionColor, targetPotionColor, std::min(1.0f, dt * 3.0f));
        if (stirrerActive) stirAngle += dt * 1.8f;

        glClearColor(0.04f, 0.04f, 0.06f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);
        shader.setMat4("u_View", view);
        shader.setMat4("u_Projection", proj);
        shader.setVec3("u_LightPos", candlesLit ? glm::vec3(1.4f, 2.3f, 1.4f) : glm::vec3(0.0f, 0.55f, 0.0f));

        // === ПРОХОД 1: НЕПРОЗРАЧНАЯ СЦЕНА ===

        // 1. Комната (пол, каменные стены и задняя полка)
        drawMesh(shader, cube, glm::scale(glm::translate(glm::mat4(1.0f), { 0.0f, -1.25f, 0.0f }), { 9.0f, 0.1f, 7.0f }), { 0.12f, 0.11f, 0.12f });
        drawMesh(shader, cube, glm::scale(glm::translate(glm::mat4(1.0f), { 0.0f,  1.2f, -2.1f }), { 9.0f, 5.0f, 0.2f }), { 0.16f, 0.15f, 0.18f });
        drawMesh(shader, cube, glm::scale(glm::translate(glm::mat4(1.0f), { -4.4f, 1.2f,  0.5f }), { 0.2f, 5.0f, 7.0f }), { 0.14f, 0.13f, 0.16f });
        drawMesh(shader, cube, glm::scale(glm::translate(glm::mat4(1.0f), { 4.4f, 1.2f,  0.5f }), { 0.2f, 5.0f, 7.0f }), { 0.14f, 0.13f, 0.16f });
        // Полка на задней стене
        drawMesh(shader, cube, glm::scale(glm::translate(glm::mat4(1.0f), { -1.3f, 1.25f, -1.82f }), { 2.4f, 0.09f, 0.45f }), { 0.26f, 0.16f, 0.10f });
        drawMesh(shader, cube, glm::scale(glm::translate(glm::mat4(1.0f), { -2.1f, 1.45f, -1.80f }), { 0.12f, 0.32f, 0.26f }), { 0.38f, 0.14f, 0.12f });
        drawMesh(shader, cube, glm::scale(glm::translate(glm::mat4(1.0f), { -1.94f,1.43f, -1.80f }), { 0.10f, 0.28f, 0.24f }), { 0.18f, 0.28f, 0.34f });

        // 2. Массивный стол (столешница y = 0.0 + 4 ножки до пола)
        drawMesh(shader, cube, glm::scale(glm::translate(glm::mat4(1.0f), { 0.0f, -0.09f, 0.0f }), { 4.6f, 0.18f, 2.6f }), { 0.30f, 0.19f, 0.11f });
        float legX[2] = { -2.05f, 2.05f }, legZ[2] = { -1.05f, 1.05f };
        for (int ix = 0; ix < 2; ++ix)
            for (int iz = 0; iz < 2; ++iz)
                drawMesh(shader, cube, glm::scale(glm::translate(glm::mat4(1.0f), { legX[ix], -0.68f, legZ[iz] }), { 0.18f, 1.05f, 0.18f }), { 0.24f, 0.15f, 0.09f });

        // 3. Железная тренога под главной колбой
        const float flaskBaseY = 0.14f;
        drawMesh(shader, tripodRing, glm::translate(glm::mat4(1.0f), { 0.0f, flaskBaseY, 0.0f }), { 0.18f, 0.19f, 0.22f });
        for (int i = 0; i < 3; ++i) {
            float angle = glm::radians(i * 120.0f + 30.0f);
            glm::mat4 leg = glm::translate(glm::mat4(1.0f), { 0.34f * std::cos(angle), 0.0f, 0.34f * std::sin(angle) });
            leg = glm::rotate(leg, -angle, { 0, 1, 0 });
            leg = glm::rotate(leg, glm::radians(10.0f), { 0, 0, 1 });
            drawMesh(shader, cylinder, glm::scale(leg, { 0.045f, flaskBaseY + 0.03f, 0.045f }), { 0.16f, 0.17f, 0.20f });
        }

        // 4. Стопка алхимических книг слева и раскрытая книга справа
        glm::mat4 book1 = glm::rotate(glm::translate(glm::mat4(1.0f), { -1.55f, 0.065f, 0.15f }), glm::radians(14.0f), { 0, 1, 0 });
        drawMesh(shader, cube, glm::scale(book1, { 0.65f, 0.13f, 0.85f }), { 0.42f, 0.13f, 0.11f });
        glm::mat4 book2 = glm::rotate(glm::translate(glm::mat4(1.0f), { -1.52f, 0.185f, 0.12f }), glm::radians(-6.0f), { 0, 1, 0 });
        drawMesh(shader, cube, glm::scale(book2, { 0.56f, 0.11f, 0.76f }), { 0.16f, 0.25f, 0.38f });
        glm::mat4 rightBook = glm::rotate(glm::translate(glm::mat4(1.0f), { 1.48f, 0.04f, 0.35f }), glm::radians(-18.0f), { 0, 1, 0 });
        drawMesh(shader, cube, glm::scale(rightBook, { 0.72f, 0.08f, 0.54f }), { 0.78f, 0.71f, 0.55f });

        // 5. Подсвечники, свечи и анимированные язычки пламени
        struct CandleDef { glm::vec3 pos; float h; };
        CandleDef candles[3] = {
            {{ 1.45f, 0.0f, -0.45f }, 0.46f},
            {{ 1.65f, 0.0f, -0.28f }, 0.32f},
            {{-1.55f, 0.0f, -0.60f }, 0.38f}
        };
        for (int i = 0; i < 3; ++i) {
            glm::mat4 base = glm::translate(glm::mat4(1.0f), candles[i].pos);
            drawMesh(shader, cylinder, glm::scale(base, { 0.26f, 0.03f, 0.26f }), { 0.22f, 0.19f, 0.15f }); // Блюдце
            drawMesh(shader, cylinder, glm::scale(glm::translate(base, { 0, 0.03f, 0 }), { 0.16f, candles[i].h, 0.16f }), { 0.90f, 0.85f, 0.72f });
            drawMesh(shader, cylinder, glm::scale(glm::translate(base, { 0, 0.03f + candles[i].h, 0 }), { 0.015f, 0.04f, 0.015f }), { 0.10f, 0.08f, 0.08f }); // Фитиль

            if (candlesLit) {
                float flicker = 1.0f + 0.12f * std::sin(now * (9.0f + i * 2.7f)) + 0.07f * std::cos(now * 19.0f);
                glm::mat4 flame = glm::translate(base, { 0.0f, 0.09f + candles[i].h, 0.0f });
                flame = glm::scale(flame, { 0.055f * flicker, 0.13f * flicker, 0.055f * flicker });
                drawMesh(shader, sphere, flame, { 1.0f, 0.65f, 0.15f }, 1.0f, 1.0f);
            }
        }

        // 6. Деревянная подставка и 3 маленькие колбы с реагентами (пробки + внутренняя жидкость)
        glm::vec3 rackCenter(-0.98f, 0.0f, -0.58f);
        drawMesh(shader, cube, glm::scale(glm::translate(glm::mat4(1.0f), rackCenter + glm::vec3(0, 0.025f, 0)), { 0.96f, 0.05f, 0.34f }), { 0.25f, 0.15f, 0.09f });
        glm::vec3 reagentColors[3] = { {0.95f, 0.15f, 0.20f}, {0.15f, 0.35f, 0.98f}, {0.15f, 0.95f, 0.35f} };
        glm::vec3 vialPositions[3] = {
            rackCenter + glm::vec3(-0.30f, 0.05f, 0.0f),
            rackCenter + glm::vec3(0.00f, 0.05f, 0.0f),
            rackCenter + glm::vec3(0.30f, 0.05f, 0.0f)
        };
        for (int i = 0; i < 3; ++i) {
            glm::mat4 vm = glm::translate(glm::mat4(1.0f), vialPositions[i]);
            // Пробка сверху
            drawMesh(shader, cylinder, glm::scale(glm::translate(vm, { 0.0f, 0.39f, 0.0f }), { 0.085f, 0.075f, 0.085f }), { 0.48f, 0.32f, 0.18f });
            // Цветная жидкость внутри маленькой колбы (заполнена на 75%)
            drawMesh(shader, smallVialLiquid, vm, reagentColors[i], 1.0f, 0.45f, 0.20f);
            float smallTopR = sampleProfileRadius(smallLiquidProfile, 0.20f);
            drawMesh(shader, disk, glm::scale(glm::translate(vm, { 0.0f, 0.20f, 0.0f }), { smallTopR, 1.0f, smallTopR }), reagentColors[i], 1.0f, 0.55f);
        }

        // 7. Иерархия Parent -> Child (R3): механический алхимический штатив-мешалка
        glm::mat4 standBase = glm::translate(glm::mat4(1.0f), { 0.88f, 0.0f, 0.0f });
        drawMesh(shader, cube, glm::scale(glm::translate(standBase, { 0, 0.05f, 0 }), { 0.42f, 0.10f, 0.42f }), { 0.22f, 0.15f, 0.10f });
        drawMesh(shader, cylinder, glm::scale(standBase, { 0.11f, 1.52f, 0.11f }), { 0.30f, 0.22f, 0.15f });

        glm::mat4 armPivot = glm::translate(standBase, { 0.0f, 1.44f, 0.0f });
        drawMesh(shader, cube, glm::scale(glm::translate(armPivot, { -0.44f, 0.0f, 0.0f }), { 0.98f, 0.08f, 0.12f }), { 0.38f, 0.25f, 0.15f });

        glm::mat4 rotorPivot = glm::rotate(glm::translate(armPivot, { -0.88f, 0.0f, 0.0f }), stirAngle, { 0.0f, 1.0f, 0.0f });
        drawMesh(shader, cylinder, glm::scale(glm::translate(rotorPivot, { 0.0f, -1.10f, 0.0f }), { 0.045f, 1.10f, 0.045f }), { 0.68f, 0.56f, 0.36f });
        drawMesh(shader, cube, glm::scale(glm::translate(rotorPivot, { 0.0f, -1.07f, 0.0f }), { 0.28f, 0.08f, 0.035f }), { 0.68f, 0.56f, 0.36f });

        // 8. Жидкость внутри главной пузатой колбы (рисуем ДО прозрачного стекла)
        glm::mat4 mainFlaskM = glm::translate(glm::mat4(1.0f), { 0.0f, flaskBaseY, 0.0f });
        if (liquidLevel > 0.01f) {
            float clipHeight = glm::mix(0.06f, 0.68f, liquidLevel);
            drawMesh(shader, mainFlaskLiquid, mainFlaskM, currentPotionColor, 1.0f, 0.65f, clipHeight);

            float surfaceR = sampleProfileRadius(mainLiquidProfile, clipHeight);
            glm::mat4 meniscusM = glm::scale(glm::translate(mainFlaskM, { 0.0f, clipHeight, 0.0f }), { surfaceR, 1.0f, surfaceR });
            drawMesh(shader, disk, meniscusM, currentPotionColor * 1.15f, 1.0f, 0.80f);
        }

        // === ПРОХОД 2: ПОЛУПРОЗРАЧНОЕ СТЕКЛО (с отключённой записью в Z-буфер) ===
        glDepthMask(GL_FALSE);

        // Декоративные бутылки на задней полке
        drawMesh(shader, smallVialGlass, glm::scale(glm::translate(glm::mat4(1.0f), { -1.25f, 1.30f, -1.80f }), { 1.3f, 1.3f, 1.3f }), { 0.70f, 0.85f, 0.90f }, 0.30f);
        drawMesh(shader, smallVialGlass, glm::scale(glm::translate(glm::mat4(1.0f), { -0.80f, 1.30f, -1.78f }), { 1.1f, 1.5f, 1.1f }), { 0.75f, 0.90f, 0.80f }, 0.30f);

        // Стеклянные оболочки трёх маленьких колб с реагентами
        for (int i = 0; i < 3; ++i) {
            glm::mat4 vm = glm::translate(glm::mat4(1.0f), vialPositions[i]);
            drawMesh(shader, smallVialGlass, vm, { 0.82f, 0.92f, 1.0f }, 0.28f, 0.15f);
        }

        // Стеклянная оболочка главной пузатой колбы
        drawMesh(shader, mainFlaskGlass, mainFlaskM, { 0.82f, 0.92f, 1.0f }, 0.26f, 0.15f);

        glDepthMask(GL_TRUE);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}