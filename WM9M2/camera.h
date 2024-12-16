#pragma once

#include "mathLib.h" 
#include <cmath>    
#include <windows.h> 
#include <dinput.h>  

class Camera {
public:
    mathLib::Vec3 position;   
    mathLib::Vec3 front;      
    mathLib::Vec3 up;         
    mathLib::Vec3 right;     
    float yaw;                
    float pitch;             
    float speed;              
    float sensitivity;        


    mathLib::Vec3 armPositionOffset; 
    float armYawOffset;      
    float armPitchOffset;  
    float armRollOffset;   

    Camera(mathLib::Vec3 startPosition = mathLib::Vec3(0.0f, 0.0f, 0.0f))
        : position(startPosition),
        yaw(-90.0f), pitch(0.0f),
        speed(10.0f), sensitivity(0.01f),
        armPositionOffset(1.0f, -0.1f, 0.0f),  
        armYawOffset(0.0f),
        armPitchOffset(0.0f),
        armRollOffset(0.0f)
    {
        updateCameraVectors();
        updateRotationMatrix();
    }

    // 更新摄像机视图矩阵
    mathLib::Matrix getViewMatrix() {
        mathLib::Vec3 to = position + front;
        return mathLib::lookAt(position, to, up);
    }

    mathLib::Matrix getArmTransform() {
        // 手臂相对于摄像机的偏移量（调整这个位置来放置手臂）
        mathLib::Vec3 armOffset = right * 1.0f + up * 0.0f + front * 0.0f;
        mathLib::Vec3 armPosition = position + armOffset;

        mathLib::Matrix translation = mathLib::Matrix::translation(armPosition);
        return translation * mathLib::Matrix::rotationYawPitch(yaw, pitch);
    }


    // 键盘输入控制摄像机移动
    void processKeyboard(float deltaTime, bool moveForward, bool moveBackward, bool moveLeft, bool moveRight) {
        float velocity = speed * deltaTime;

        if (moveForward) position = position + front * velocity;
        if (moveBackward) position = position - front * velocity;
        if (moveLeft) position = position - right * velocity;
        if (moveRight) position = position + right * velocity;
    }

    // 鼠标输入控制摄像机视角
    void processMouseMovement(float xOffset, float yOffset) {
        xOffset *= sensitivity;
        yOffset *= sensitivity;

        yaw += xOffset;
        pitch += yOffset;

        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        updateCameraVectors();
        updateRotationMatrix();
    }

private:
    // 更新前向向量、右向向量和上向向量
    mathLib::Matrix cameraRotationMatrix;

    void updateCameraVectors() {
        mathLib::Vec3 newFront;
        newFront.x = cos(mathLib::radians(yaw)) * cos(mathLib::radians(pitch));
        newFront.y = sin(mathLib::radians(pitch));
        newFront.z = sin(mathLib::radians(yaw)) * cos(mathLib::radians(pitch));
        front = newFront.normalize();

        right = (front.cross(mathLib::Vec3(0.0f, 1.0f, 0.0f))).normalize();
        up = right.cross(front).normalize();
    }

    void updateRotationMatrix() {
        cameraRotationMatrix = mathLib::Matrix::rotationYawPitch(
            mathLib::radians(yaw),
            mathLib::radians(pitch)
         );
    }

};
