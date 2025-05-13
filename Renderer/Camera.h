#pragma once
#include <freeglut.h>
#include <cmath>

// Define PI constant if not already defined by the compiler
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class Camera {
public:
    // Constructor with default values
    Camera(float posX = 0.0f, float posY = 0.0f, float posZ = 5.0f);
    
    // Reset camera to default position and orientation
    void reset();
    
    // Update camera position based on movement inputs
    void updatePosition(float forwardAmount, float rightAmount, float upAmount);
    
    // Update camera orientation based on mouse movement
    void updateOrientation(float yawDelta, float pitchDelta);
    
    // Set up camera view for rendering
    void setupView();
    
    // Gets and sets
    float getX() const { return posX; }
    float getY() const { return posY; }
    float getZ() const { return posZ; }
    float getYaw() const { return yaw; }
    float getPitch() const { return pitch; }
    float getSpeed() const { return moveSpeed; }
    float getSensitivity() const { return mouseSensitivity; }
    
    void setX(float x) { posX = x; }
    void setY(float y) { posY = y; }
    void setZ(float z) { posZ = z; }
    void setYaw(float y) { yaw = y; }
    void setPitch(float p) { pitch = p; }
    void setSpeed(float speed) { moveSpeed = speed; }
    void setSensitivity(float sensitivity) { mouseSensitivity = sensitivity; }
    
private:
    // Camera position
    float posX, posY, posZ;
    
    // Camera orientation angles
    float yaw, pitch;
    
    // Camera settings
    float moveSpeed;
    float mouseSensitivity;
};

// Global camera instance for use throughout the application
extern Camera camera;