#pragma once
#include "mathLib.h"
#include "animation.h"
#include "collision.h"
#include <string>

// T-Rex Player Controller with Collision
class TRexPlayer {
public:
    mathLib::Vec3 position;
    mathLib::Vec3 velocity;
    float speed;
    float rotationY;
    LoadAnimation* model;
    std::string currentAnimation;
    bool isMoving;

    // Collision
    AABB boundingBox;
    float boundingBoxWidth;
    float boundingBoxHeight;
    float boundingBoxDepth;

    TRexPlayer(const mathLib::Vec3& startPos = mathLib::Vec3(0, 0, 0), float moveSpeed = 8.0f)
        : position(startPos), velocity(0, 0, 0), speed(moveSpeed),
        rotationY(0), model(nullptr), currentAnimation("Idle"), isMoving(false),
        boundingBoxWidth(2.0f), boundingBoxHeight(3.0f), boundingBoxDepth(4.0f) {
        updateBoundingBox();
    }

    void bindModel(LoadAnimation* trexModel) {
        model = trexModel;
    }

    void update(const mathLib::Vec3& moveDirection, float dt, CollisionWorld* collisionWorld = nullptr) {
        // Create non-const copy for getLengthSquare() which is not const
        mathLib::Vec3 moveDir = moveDirection;

        if (moveDir.getLengthSquare() > 0.001f) {
            // Calculate target rotation from movement direction
            float targetRotation = atan2f(moveDir.x, moveDir.z);

            // Smooth rotation
            float rotationDiff = targetRotation - rotationY;
            while (rotationDiff > M_PI) rotationDiff -= 2 * M_PI;
            while (rotationDiff < -M_PI) rotationDiff += 2 * M_PI;

            rotationY += rotationDiff * 10.0f * dt;

            // Calculate velocity
            mathLib::Vec3 vel = moveDir * speed;
            velocity = vel;

            // Calculate new position
            mathLib::Vec3 newPosition = position + vel * dt;

            // Check collision if collision world exists
            if (collisionWorld) {
                // Create AABB at new position
                AABB newBox = boundingBox;
                mathLib::Vec3 offset = newPosition - position;
                newBox.minPoint = newBox.minPoint + offset;  
                newBox.maxPoint = newBox.maxPoint + offset;  

                // Check and resolve collisions
                mathLib::Vec3 responseVel = collisionWorld->checkCollision(newBox, vel);

                // Apply collision response
                if (responseVel.x != vel.x || responseVel.z != vel.z) {
                    // Collision detected, use sliding response
                    newPosition = position + responseVel * dt;
                }
            }

            position = newPosition;
            position.y = 0.0f;  // Keep on ground

            updateBoundingBox();

            isMoving = true;
            currentAnimation = "Run";
        }
        else {
            isMoving = false;
            currentAnimation = "Idle";
        }
    }

    void updateBoundingBox() {
        // Center the bounding box around the player position
        boundingBox.minPoint = position - mathLib::Vec3(boundingBoxWidth * 0.5f, 0, boundingBoxDepth * 0.5f);  // 修改：使用 minPoint
        boundingBox.maxPoint = position + mathLib::Vec3(boundingBoxWidth * 0.5f, boundingBoxHeight, boundingBoxDepth * 0.5f);  // 修改：使用 maxPoint
    }

    mathLib::Matrix getWorldMatrix() {
        mathLib::Matrix scale = mathLib::Matrix::scaling(mathLib::Vec3(1.5f, 1.5f, 1.5f));
        mathLib::Matrix rotation = mathLib::Matrix::rotateY(rotationY);
        mathLib::Matrix translation = mathLib::Matrix::translation(position);
        return scale * rotation * translation;
    }

    AABB getBoundingBox() const {
        return boundingBox;
    }
};