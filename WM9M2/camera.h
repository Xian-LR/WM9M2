#pragma once
#include "mathLib.h"
#include <cmath>

// Forward declaration
class TRexPlayer;

// Third-Person Camera System
class TPSCamera {
public:
    mathLib::Vec3 position;
    mathLib::Vec3 target;
    mathLib::Vec3 up;

    float distance;     
    float height;        
    float yaw;          
    float pitch;        

    float mouseSensitivity;
    float smoothness;  

    TRexPlayer* player;

    TPSCamera(TRexPlayer* targetPlayer = nullptr, float dist = 15.0f, float h = 8.0f)
        : player(targetPlayer), distance(dist), height(h),
        yaw(0), pitch(-25.0f), mouseSensitivity(0.15f), smoothness(8.0f) {
        up = mathLib::Vec3(0, 1, 0);
        position = mathLib::Vec3(0, height, distance);
        target = mathLib::Vec3(0, 0, 0);
    }

    void bindPlayer(TRexPlayer* targetPlayer) {
        player = targetPlayer;
    }

    mathLib::Vec3 getCameraPos() const {
        return position;
    }


    void processMouseMovement(float dx, float dy) {
        dx *= mouseSensitivity;
        dy *= mouseSensitivity;

        yaw += dx;  
        pitch -= dy;

        if (pitch > -5.0f) pitch = -5.0f;
        if (pitch < -60.0f) pitch = -60.0f;

        while (yaw > 360.0f) yaw -= 360.0f;
        while (yaw < 0.0f) yaw += 360.0f;
    }

    void updatePosition(const mathLib::Vec3& playerPos, float dt) {
        float radYaw = mathLib::radians(yaw);
        float radPitch = mathLib::radians(pitch);

        float offsetX = distance * std::cos(radPitch) * std::sin(radYaw);
        float offsetY = height - distance * std::sin(radPitch);
        float offsetZ = distance * std::cos(radPitch) * std::cos(radYaw);

        mathLib::Vec3 desiredPosition = playerPos + mathLib::Vec3(offsetX, offsetY, offsetZ);

        float lerpFactor = 1.0f - std::exp(-smoothness * dt);
        position = position + (desiredPosition - position) * lerpFactor;

        target = playerPos + mathLib::Vec3(0, 1.5f, 0);
    }

    mathLib::Matrix getViewMatrix() {
        mathLib::Vec3 eye = position;
        mathLib::Vec3 lookTarget = target;
        mathLib::Vec3 upVec = up;
        return mathLib::lookAt(eye, lookTarget, upVec);
    }

    mathLib::Vec3 getCameraForward() const {
        float radYaw = mathLib::radians(yaw);
        return mathLib::Vec3(-std::sin(radYaw), 0, -std::cos(radYaw));
    }

    mathLib::Vec3 getCameraRight() const {
        float radYaw = mathLib::radians(yaw);
        return mathLib::Vec3(std::cos(radYaw), 0, -std::sin(radYaw));
    }
};