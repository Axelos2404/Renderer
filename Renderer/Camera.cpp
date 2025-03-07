#include "Camera.h"
#include <algorithm>

// Define PI constant if not already defined by the compiler
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Camera parameters with default values
float cameraX = 0.0f, cameraY = 0.0f, cameraZ = 5.0f;                                    // Camera position
float cameraYaw = 0.0f, cameraPitch = 0.0f;                                              // Camera orientation angles
float cameraSpeed = 0.1f;                                                                // Camera movement speed
float mouseSensitivity = 0.2f;                                                           // Mouse sensitivity for camera control

// Reset camera to default position and orientation
void resetCamera() {
    cameraX = 0.0f;                                                                      // Reset X position
    cameraY = 0.0f;                                                                      // Reset Y position
    cameraZ = 5.0f;                                                                      // Reset Z position
    cameraYaw = 0.0f;                                                                    // Reset yaw angle
    cameraPitch = 0.0f;                                                                  // Reset pitch angle
}

// Update camera position based on movement inputs
void updateCameraPosition(float forwardAmount, float rightAmount, float upAmount) {
    // Calculate forward and right vectors based on camera orientation
    float forwardX = sin(cameraYaw * M_PI / 180.0f) * cos(cameraPitch * M_PI / 180.0f);  // X component of forward vector
    float forwardY = sin(cameraPitch * M_PI / 180.0f);                                   // Y component of forward vector
    float forwardZ = cos(cameraYaw * M_PI / 180.0f) * cos(cameraPitch * M_PI / 180.0f);  // Z component of forward vector

    // Right vector is perpendicular to forward vector (cross product with up vector)
    float rightX = sin((cameraYaw + 90.0f) * M_PI / 180.0f);                             // X component of right vector
    float rightZ = cos((cameraYaw + 90.0f) * M_PI / 180.0f);                             // Z component of right vector

    // Update camera position based on movement inputs
    cameraX += forwardX * forwardAmount * cameraSpeed;                                   // Move along forward vector X
    cameraY += forwardY * forwardAmount * cameraSpeed;                                   // Move along forward vector Y
    cameraZ += forwardZ * forwardAmount * cameraSpeed;                                   // Move along forward vector Z

    cameraX += rightX * rightAmount * cameraSpeed;                                       // Move along right vector X
    cameraZ += rightZ * rightAmount * cameraSpeed;                                       // Move along right vector Z

    cameraY += upAmount * cameraSpeed;                                                   // Move directly up/down
}

// Update camera orientation based on mouse movement
void updateCameraOrientation(float yawDelta, float pitchDelta) {
    // Update camera orientation
    cameraYaw -= yawDelta * mouseSensitivity;                                            // Adjust yaw (left/right rotation)
    cameraPitch -= pitchDelta * mouseSensitivity;                                        // Adjust pitch (up/down rotation)

    // Limit pitch angle to prevent camera flipping
    cameraPitch = std::max(-89.0f, std::min(89.0f, cameraPitch));                        // Clamp pitch between -89 and 89 degrees
}

// Set up camera view for rendering
void setupCamera() {
    // Calculate camera look-at point based on camera position and orientation angles
    float lookX = cameraX + sin(cameraYaw * M_PI / 180.0f) * cos(cameraPitch * M_PI / 180.0f);  // X coordinate of look-at point
    float lookY = cameraY + sin(cameraPitch * M_PI / 180.0f);                                   // Y coordinate of look-at point
    float lookZ = cameraZ + cos(cameraYaw * M_PI / 180.0f) * cos(cameraPitch * M_PI / 180.0f);  // Z coordinate of look-at point

    // Set up the camera view using the calculated look-at point
    gluLookAt(cameraX, cameraY, cameraZ,    // Camera position (eye point)
        lookX, lookY, lookZ,          // Look-at point (center of view)
        0.0f, 1.0f, 0.0f);           // Up vector (defines camera orientation)
}