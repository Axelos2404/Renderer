#include "Camera.h"
#include <algorithm>

// Define PI constant if not already defined by the compiler
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Constructor implementation
Camera::Camera(float posX, float posY, float posZ) {
    this->posX = posX;
    this->posY = posY;
    this->posZ = posZ;
    yaw = 0.0f;
    pitch = 0.0f;
    moveSpeed = 0.1f;
    mouseSensitivity = 0.2f;
}

// Reset camera to default position and orientation
void Camera::reset() {
    posX = 0.0f;
    posY = 0.0f;
    posZ = 5.0f;
    yaw = 0.0f;
    pitch = 0.0f;
}

// Update camera position based on movement inputs
void Camera::updatePosition(float forwardAmount, float rightAmount, float upAmount) {
    // Calculate forward and right vectors based on camera orientation
    float forwardX = sin(yaw * M_PI / 180.0f) * cos(pitch * M_PI / 180.0f);
    float forwardY = sin(pitch * M_PI / 180.0f);
    float forwardZ = cos(yaw * M_PI / 180.0f) * cos(pitch * M_PI / 180.0f);

    // Right vector is perpendicular to forward vector (cross product with up vector)
    float rightX = sin((yaw + 90.0f) * M_PI / 180.0f);
    float rightZ = cos((yaw + 90.0f) * M_PI / 180.0f);

    // Update camera position based on movement inputs
    posX += forwardX * forwardAmount * moveSpeed;
    posY += forwardY * forwardAmount * moveSpeed;
    posZ += forwardZ * forwardAmount * moveSpeed;

    posX += rightX * rightAmount * moveSpeed;
    posZ += rightZ * rightAmount * moveSpeed;

    posY += upAmount * moveSpeed;
}

// Update camera orientation based on mouse movement
void Camera::updateOrientation(float yawDelta, float pitchDelta) {
    // Update camera orientation
    yaw += yawDelta * mouseSensitivity;
    pitch += pitchDelta * mouseSensitivity;

    // Limit pitch angle to prevent camera flipping
    pitch = std::max(-89.0f, std::min(89.0f, pitch));
}

// Set up camera view for rendering
void Camera::setupView() {
    // Calculate camera look-at point based on camera position and orientation angles
    float lookX = posX + sin(yaw * M_PI / 180.0f) * cos(pitch * M_PI / 180.0f);
    float lookY = posY + sin(pitch * M_PI / 180.0f);
    float lookZ = posZ + cos(yaw * M_PI / 180.0f) * cos(pitch * M_PI / 180.0f);

    // Set up the camera view using the calculated look-at point
    gluLookAt(posX, posY, posZ,    // Camera position (eye point)
        lookX, lookY, lookZ,        // Look-at point (center of view)
        0.0f, 1.0f, 0.0f);          // Up vector (defines camera orientation)
}