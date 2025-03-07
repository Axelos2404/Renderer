#pragma once
#include <freeglut.h>
#include <cmath>

// Camera parameters
extern float cameraX, cameraY, cameraZ;                                                  // Camera position
extern float cameraYaw, cameraPitch;                                                     // Camera orientation angles
extern float cameraSpeed;                                                                // Camera movement speed
extern float mouseSensitivity;                                                           // Mouse sensitivity for camera control

// Function declarations
void resetCamera();                                                                      // Reset camera to default position and orientation
void updateCameraPosition(float forwardAmount, float rightAmount, float upAmount);       // Update camera position based on movement inputs
void updateCameraOrientation(float yawDelta, float pitchDelta);                          // Update camera orientation based on mouse movement
void setupCamera();                                                                      // Set up camera view for rendering