#pragma once
#include "mathLib.h"
#include <vector>
#include <cfloat>
#include <string>
#include <algorithm>

#ifndef NOMINMAX
#define NOMINMAX
#endif

// Axis-Aligned Bounding Box
class AABB {
public:
    mathLib::Vec3 minPoint; 
    mathLib::Vec3 maxPoint;  

    AABB() {
        minPoint = mathLib::Vec3(FLT_MAX, FLT_MAX, FLT_MAX);
        maxPoint = mathLib::Vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    }

    AABB(const mathLib::Vec3& minPt, const mathLib::Vec3& maxPt)
        : minPoint(minPt), maxPoint(maxPt) {  
    }

    // Check if this AABB intersects with another
    bool intersects(const AABB& other) const {
        return (minPoint.x <= other.maxPoint.x && maxPoint.x >= other.minPoint.x) &&
            (minPoint.y <= other.maxPoint.y && maxPoint.y >= other.minPoint.y) &&
            (minPoint.z <= other.maxPoint.z && maxPoint.z >= other.minPoint.z);
    }

    // Get center of AABB
    mathLib::Vec3 getCenter() const {
        return (minPoint + maxPoint) * 0.5f;
    }

    // Get size of AABB
    mathLib::Vec3 getSize() const {
        return mathLib::Vec3(maxPoint.x - minPoint.x, maxPoint.y - minPoint.y, maxPoint.z - minPoint.z);
    }

    // Expand AABB to include a point
    void expand(const mathLib::Vec3& point) {
        minPoint.x = (point.x < minPoint.x) ? point.x : minPoint.x;
        minPoint.y = (point.y < minPoint.y) ? point.y : minPoint.y;
        minPoint.z = (point.z < minPoint.z) ? point.z : minPoint.z;

        maxPoint.x = (point.x > maxPoint.x) ? point.x : maxPoint.x;
        maxPoint.y = (point.y > maxPoint.y) ? point.y : maxPoint.y;
        maxPoint.z = (point.z > maxPoint.z) ? point.z : maxPoint.z;
    }

    // Transform AABB by a matrix
    AABB transform(const mathLib::Matrix& matrix) const {
        // Transform all 8 corners of the AABB
        mathLib::Vec3 corners[8] = {
            mathLib::Vec3(minPoint.x, minPoint.y, minPoint.z),
            mathLib::Vec3(maxPoint.x, minPoint.y, minPoint.z),
            mathLib::Vec3(minPoint.x, maxPoint.y, minPoint.z),
            mathLib::Vec3(maxPoint.x, maxPoint.y, minPoint.z),
            mathLib::Vec3(minPoint.x, minPoint.y, maxPoint.z),
            mathLib::Vec3(maxPoint.x, minPoint.y, maxPoint.z),
            mathLib::Vec3(minPoint.x, maxPoint.y, maxPoint.z),
            mathLib::Vec3(maxPoint.x, maxPoint.y, maxPoint.z)
        };

        AABB result;
        for (int i = 0; i < 8; i++) {
            mathLib::Vec3 transformed = matrix.mulPoint(corners[i]);
            result.expand(transformed);
        }

        return result;
    }
};

// Collision World manages all collidable objects
class CollisionWorld {
public:
    struct CollisionObject {
        AABB boundingBox;
        bool isStatic;  // Static objects don't move
        std::string name;

        CollisionObject(const AABB& box, bool static_obj = true, const std::string& n = "")
            : boundingBox(box), isStatic(static_obj), name(n) {
        }
    };

private:
    std::vector<CollisionObject> objects;

public:
    // Add a collision object
    void addObject(const AABB& box, bool isStatic = true, const std::string& name = "") {
        objects.push_back(CollisionObject(box, isStatic, name));
    }

    // Clear all objects
    void clear() {
        objects.clear();
    }

    // Check collision and get response
    mathLib::Vec3 checkCollision(const AABB& movingBox, const mathLib::Vec3& velocity) {
        mathLib::Vec3 responseVelocity = velocity;

        for (const auto& obj : objects) {
            if (obj.isStatic && movingBox.intersects(obj.boundingBox)) {
                // Calculate penetration depth on each axis
                mathLib::Vec3 penetration = calculatePenetration(movingBox, obj.boundingBox);

                // Find axis with minimum penetration (separation axis)
                float minPen = FLT_MAX;
                int axis = -1;

                if (std::abs(penetration.x) < minPen && std::abs(velocity.x) > 0.001f) {
                    minPen = std::abs(penetration.x);
                    axis = 0;
                }
                if (std::abs(penetration.y) < minPen && std::abs(velocity.y) > 0.001f) {
                    minPen = std::abs(penetration.y);
                    axis = 1;
                }
                if (std::abs(penetration.z) < minPen && std::abs(velocity.z) > 0.001f) {
                    minPen = std::abs(penetration.z);
                    axis = 2;
                }

                // Apply response on the minimum penetration axis
                if (axis == 0) responseVelocity.x = 0;
                if (axis == 1) responseVelocity.y = 0;
                if (axis == 2) responseVelocity.z = 0;
            }
        }

        return responseVelocity;
    }

    // Get all collision objects (for debug rendering)
    const std::vector<CollisionObject>& getObjects() const {
        return objects;
    }

private:
    mathLib::Vec3 calculatePenetration(const AABB& box1, const AABB& box2) {
        mathLib::Vec3 penetration;

        // Calculate overlap on each axis
        penetration.x = (std::min)(box1.maxPoint.x - box2.minPoint.x, box2.maxPoint.x - box1.minPoint.x);
        penetration.y = (std::min)(box1.maxPoint.y - box2.minPoint.y, box2.maxPoint.y - box1.minPoint.y);
        penetration.z = (std::min)(box1.maxPoint.z - box2.minPoint.z, box2.maxPoint.z - box1.minPoint.z);

        return penetration;
    }
};