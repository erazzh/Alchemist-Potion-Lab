#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

enum class CameraMove { FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN };

class Camera {
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float Yaw;
    float Pitch;
    float Speed = 3.5f;
    float Sensitivity = 0.1f;

    Camera(glm::vec3 startPos = { 0.0f, 1.8f, 4.2f }, float yaw = -90.0f, float pitch = -15.0f)
        : Position(startPos), WorldUp(0.0f, 1.0f, 0.0f), Yaw(yaw), Pitch(pitch) {
        updateVectors();
    }

    glm::mat4 GetViewMatrix() const {
        return glm::lookAt(Position, Position + Front, Up);
    }

    void ProcessKeyboard(CameraMove dir, float dt) {
        float vel = Speed * dt;
        if (dir == CameraMove::FORWARD)  Position += Front * vel;
        if (dir == CameraMove::BACKWARD) Position -= Front * vel;
        if (dir == CameraMove::LEFT)     Position -= Right * vel;
        if (dir == CameraMove::RIGHT)    Position += Right * vel;
        if (dir == CameraMove::UP)       Position += WorldUp * vel;
        if (dir == CameraMove::DOWN)     Position -= WorldUp * vel;
    }

    void ProcessMouse(float xoffset, float yoffset) {
        Yaw += xoffset * Sensitivity;
        Pitch += yoffset * Sensitivity;
        Pitch = glm::clamp(Pitch, -89.0f, 89.0f);
        updateVectors();
    }

private:
    void updateVectors() {
        glm::vec3 f;
        f.x = std::cos(glm::radians(Yaw)) * std::cos(glm::radians(Pitch));
        f.y = std::sin(glm::radians(Pitch));
        f.z = std::sin(glm::radians(Yaw)) * std::cos(glm::radians(Pitch));
        Front = glm::normalize(f);
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }
};