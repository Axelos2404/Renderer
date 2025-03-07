#include "InputHandler.h"
#include "Camera.h"
#include "ModelLoader.h"
#include "Renderer.h"
#include <algorithm>

// Define PI constant if not already defined by the compiler
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Mouse state variables
bool mousePressed = false;                                                               // Flag indicating if mouse button is pressed
int mouseX = 0, mouseY = 0;                                                              // Current mouse position

// Menu ID
int mainMenu;                                                                            // ID for the main context menu

// Keyboard callback function - processes key presses for navigation and model manipulation
void keyboard(unsigned char key, int x, int y) {
    // Calculate forward and right vectors based on camera orientation
    float forwardX = sin(cameraYaw * M_PI / 180.0f) * cos(cameraPitch * M_PI / 180.0f);  // X component of forward vector
    float forwardY = sin(cameraPitch * M_PI / 180.0f);                                   // Y component of forward vector
    float forwardZ = cos(cameraYaw * M_PI / 180.0f) * cos(cameraPitch * M_PI / 180.0f);  // Z component of forward vector

    // Right vector is perpendicular to forward vector (cross product with up vector)
    float rightX = sin((cameraYaw + 90.0f) * M_PI / 180.0f);                             // X component of right vector
    float rightZ = cos((cameraYaw + 90.0f) * M_PI / 180.0f);                             // Z component of right vector

    // Model transformation speed constants
    const float moveSpeed = 0.1f;                                                        // Model translation speed
    const float rotateSpeed = 5.0f;                                                      // Model rotation speed in degrees
    const float scaleSpeed = 0.1f;                                                       // Model scaling speed

    switch (key) {
        // Camera movement controls
    case 'w':                                                                            // Move camera forward
        cameraX += forwardX * cameraSpeed;                                               // Increment X position along forward vector
        cameraY += forwardY * cameraSpeed;                                               // Increment Y position along forward vector
        cameraZ += forwardZ * cameraSpeed;                                               // Increment Z position along forward vector
        break;
    case 's':                                                                            // Move camera backward
        cameraX -= forwardX * cameraSpeed;                                               // Decrement X position along forward vector
        cameraY -= forwardY * cameraSpeed;                                               // Decrement Y position along forward vector
        cameraZ -= forwardZ * cameraSpeed;                                               // Decrement Z position along forward vector
        break;
    case 'd':                                                                            // Strafe camera right
        cameraX -= rightX * cameraSpeed;                                                 // Decrement X position along right vector
        cameraZ -= rightZ * cameraSpeed;                                                 // Decrement Z position along right vector
        break;
    case 'a':                                                                            // Strafe camera left
        cameraX += rightX * cameraSpeed;                                                 // Increment X position along right vector
        cameraZ += rightZ * cameraSpeed;                                                 // Increment Z position along right vector
        break;
    case 'q':                                                                            // Move camera up
        cameraY += cameraSpeed;                                                          // Increment Y position directly
        break;
    case 'e':                                                                            // Move camera down
        cameraY -= cameraSpeed;                                                          // Decrement Y position directly
        break;

        // Model position controls
    case 'i':                                                                            // Move model forward (away from camera)
        modelZ -= moveSpeed;                                                             // Decrement model Z position
        break;
    case 'k':                                                                            // Move model backward (toward camera)
        modelZ += moveSpeed;                                                             // Increment model Z position
        break;
    case 'j':                                                                            // Move model left
        modelX -= moveSpeed;                                                             // Decrement model X position
        break;
    case 'l':                                                                            // Move model right
        modelX += moveSpeed;                                                             // Increment model X position
        break;
    case 'u':                                                                            // Move model up
        modelY += moveSpeed;                                                             // Increment model Y position
        break;
    case 'o':                                                                            // Move model down
        modelY -= moveSpeed;                                                             // Decrement model Y position
        break;

        // Model rotation controls
    case 'x':                                                                            // Rotate model clockwise around X axis
        modelRotX += rotateSpeed;                                                        // Increment X rotation angle
        break;
    case 'X':                                                                            // Rotate model counter-clockwise around X axis
        modelRotX -= rotateSpeed;                                                        // Decrement X rotation angle
        break;
    case 'y':                                                                            // Rotate model clockwise around Y axis
        modelRotY += rotateSpeed;                                                        // Increment Y rotation angle
        break;
    case 'Y':                                                                            // Rotate model counter-clockwise around Y axis
        modelRotY -= rotateSpeed;                                                        // Decrement Y rotation angle
        break;
    case 'z':                                                                            // Rotate model clockwise around Z axis
        modelRotZ += rotateSpeed;                                                        // Increment Z rotation angle
        break;
    case 'Z':                                                                            // Rotate model counter-clockwise around Z axis
        modelRotZ -= rotateSpeed;                                                        // Decrement Z rotation angle
        break;

        // Model scaling controls
    case '+':                                                                            // Scale model up
    case '=':                                                                            // Alternative key for scaling up (same key on keyboard)
        modelScale += scaleSpeed;                                                        // Increase model scale factor
        break;
    case '-':                                                                            // Scale model down
        modelScale = std::max(0.1f, modelScale - scaleSpeed);                            // Decrease model scale factor with minimum limit
        break;

        // Reset transformation controls
    case 'r':                                                                            // Reset model transformations
        resetModel();                                                                    // Call function to reset model position, rotation, and scale
        break;
    case 'R':                                                                            // Reset camera position and orientation
        resetCamera();                                                                   // Call function to reset camera position and orientation
        break;

    case 'g':                                                                            // Toggle grid visibility
    case 'G':
        toggleGrid();                                                                    // Toggle grid on/off
        break;

    case 27:                                                                             // ESC key (ASCII 27)
        exit(0);                                                                         // Exit the application
        break;
    }
    glutPostRedisplay();                                                                 // Request a redraw to update the display
}

// Mouse button callback function - handles mouse button press and release events
void mouseButton(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {                                                    // Check if left mouse button was pressed/released
        mousePressed = (state == GLUT_DOWN);                                             // Set mousePressed flag based on button state
        mouseX = x;                                                                      // Store current X position of mouse
        mouseY = y;                                                                      // Store current Y position of mouse
    }
}

// Mouse motion callback function - handles mouse movement when button is pressed
void mouseMotion(int x, int y) {
    if (mousePressed) {                                                                  // Only process if mouse button is pressed
        float deltaX = x - mouseX;                                                       // Calculate horizontal mouse movement
        float deltaY = y - mouseY;                                                       // Calculate vertical mouse movement

        // Update camera orientation based on mouse movement
        cameraYaw -= deltaX * mouseSensitivity;                                          // Adjust yaw (left/right rotation)
        cameraPitch -= deltaY * mouseSensitivity;                                        // Adjust pitch (up/down rotation)

        // Limit pitch angle to prevent camera flipping
        if (cameraPitch > 89.0f) cameraPitch = 89.0f;                                    // Clamp maximum pitch to 89 degrees
        if (cameraPitch < -89.0f) cameraPitch = -89.0f;                                  // Clamp minimum pitch to -89 degrees

        // Update stored mouse position for next frame
        mouseX = x;                                                                      // Store new X position
        mouseY = y;                                                                      // Store new Y position

        glutPostRedisplay();                                                             // Request a redraw to update the display
    }
}

// Menu callback function - processes menu selections
void menuCallback(int option) {
    switch (option) {
    case MENU_LOAD_MODEL:                                                                // User selected "Load New Model"
        loadNewModel();                                                                  // Call function to load a new model
        break;
    case MENU_RESET_CAMERA:                                                              // User selected "Reset Camera"
        resetCamera();                                                                   // Reset camera to default position
        break;
    case MENU_RESET_MODEL:                                                               // User selected "Reset Model Position"
        resetModel();                                                                    // Reset model transformations
        break;
    case MENU_TOGGLE_GRID:                                                               // User selected "Toggle Grid"
        toggleGrid();                                                                    // Toggle grid visibility flag
        glutPostRedisplay();                                                             // Request a redraw to update display
        break;
    case MENU_EXIT:                                                                      // User selected "Exit"
        exit(0);                                                                         // Exit the application
        break;
    }
}

// Create right-click context menu
void createMenu() {
    mainMenu = glutCreateMenu(menuCallback);                                             // Create menu with callback function
    glutAddMenuEntry("Load New Model", MENU_LOAD_MODEL);                                 // Add menu option to load a new model
    glutAddMenuEntry("Reset Camera", MENU_RESET_CAMERA);                                 // Add menu option to reset camera position
    glutAddMenuEntry("Reset Model Position", MENU_RESET_MODEL);                          // Add menu option to reset model transform
    glutAddMenuEntry("Toggle Grid", MENU_TOGGLE_GRID);                                   // Add menu option to toggle grid visibility
    glutAddMenuEntry("Exit", MENU_EXIT);                                                 // Add menu option to exit application

    glutAttachMenu(GLUT_RIGHT_BUTTON);                                                   // Attach menu to right mouse button
}