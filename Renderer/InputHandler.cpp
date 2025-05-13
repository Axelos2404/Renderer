#include "InputHandler.h"
#include "Camera.h"
#include "ModelLoader.h"
#include "Renderer.h"
#include <algorithm>

// Define PI constant
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Mouse state variables
bool mousePressed = false;                                                               // Flag indicating if mouse button is pressed
int mouseX = 0, mouseY = 0;                                                              // Current mouse position

// Menu ID
int mainMenu;                                                                            // ID for the main context menu

// Constructor
InputHandler::InputHandler(Camera& cam, ModelLoader& model, Renderer& rend)
    : camera(cam), modelLoader(model), renderer(rend),
      lastMouseX(0), lastMouseY(0), mouseLeftDown(false), mouseRightDown(false) {
}

// Initialize input handling
void InputHandler::init() {
    // Register GLUT callbacks
    glutKeyboardFunc(keyboardWrapper);
    glutSpecialFunc(specialKeysWrapper);
    glutMouseFunc(mouseButtonWrapper);
    glutMotionFunc(mouseMotionWrapper);
    
    // Create right-click context menu
    createMenu();
}

// Keyboard callback function
void InputHandler::keyboardCallback(unsigned char key, int x, int y) {
    // Calculate forward and right vectors based on camera orientation
    float forwardX = sin(camera.getYaw() * M_PI / 180.0f) * cos(camera.getPitch() * M_PI / 180.0f);
    float forwardY = sin(camera.getPitch() * M_PI / 180.0f);
    float forwardZ = cos(camera.getYaw() * M_PI / 180.0f) * cos(camera.getPitch() * M_PI / 180.0f);

    // Right vector is perpendicular to forward vector (cross product with up vector)
    float rightX = sin((camera.getYaw() + 90.0f) * M_PI / 180.0f);
    float rightZ = cos((camera.getYaw() + 90.0f) * M_PI / 180.0f);

    switch (key) {
    case 'w':                                                                            // Move camera forward
    case 'W':
        camera.setX(camera.getX() + forwardX * camera.getSpeed());                       // Move along XYZ based on forward vector
        camera.setY(camera.getY() + forwardY * camera.getSpeed());
        camera.setZ(camera.getZ() + forwardZ * camera.getSpeed());
        break;
    case 's':                                                                            // Move camera backward
    case 'S':
        camera.setX(camera.getX() - forwardX * camera.getSpeed()); 
        camera.setY(camera.getY() - forwardY * camera.getSpeed()); 
        camera.setZ(camera.getZ() - forwardZ * camera.getSpeed()); 
        break;
    case 'a':                                                                            // Move camera left
    case 'A':
        camera.setX(camera.getX() + rightX * camera.getSpeed());                         // Move along XZ based on right vector
        camera.setZ(camera.getZ() + rightZ * camera.getSpeed());
        break;
    case 'd':                                                                            // Move camera right
    case 'D':
        camera.setX(camera.getX() - rightX * camera.getSpeed());
        camera.setZ(camera.getZ() - rightZ * camera.getSpeed());
        break;
    case 'q':                                                                            // Move camera up
    case 'Q':
        camera.setY(camera.getY() + camera.getSpeed());                                  // Move up along Y axis
        break;
    case 'e':                                                                            // Move camera down
    case 'E':
        camera.setY(camera.getY() - camera.getSpeed());                                  // Move down along Y axis
        break;
    case 'i':                                                                            // Move model forward (away from camera)
    case 'I':
        modelLoader.setZ(modelLoader.getZ() - 0.1f);                                     // Decrease Z position (move away)
        break;
    case 'k':                                                                            // Move model backward (toward camera)
    case 'K':
        modelLoader.setZ(modelLoader.getZ() + 0.1f);                                     // Increase Z position (move closer)
        break;
    case 'j':                                                                            // Move model left
    case 'J':
        modelLoader.setX(modelLoader.getX() - 0.1f);                                     // Decrease X position (move left)
        break;
    case 'l':                                                                            // Move model right
    case 'L':
        modelLoader.setX(modelLoader.getX() + 0.1f);                                     // Increase X position (move right)
        break;
    case 'u':                                                                            // Move model up
    case 'U':
        modelLoader.setY(modelLoader.getY() + 0.1f);                                     // Increase Y position (move up)
        break;
    case 'o':                                                                            // Move model down
    case 'O':
        modelLoader.setY(modelLoader.getY() - 0.1f);                                     // Decrease Y position (move down)
        break;
    case 'r':                                                                            // Rotate model around X axis
    case 'R':
        modelLoader.setRotX(modelLoader.getRotX() + 5.0f);                               // Increase X rotation by 5 degrees
        break;
    case 'f':                                                                            // Rotate model around Y axis
    case 'F':
        modelLoader.setRotY(modelLoader.getRotY() + 5.0f);                               // Increase Y rotation by 5 degrees
        break;
    case 'v':                                                                            // Rotate model around Z axis
    case 'V':
        modelLoader.setRotZ(modelLoader.getRotZ() + 5.0f);                               // Increase Z rotation by 5 degrees
        break;
    case '+':                                                                            // Scale model up
    case '=':
        modelLoader.setScale(modelLoader.getScale() * 1.1f);                             // Increase scale by 10%
        break;
    case '-':                                                                            // Scale model down
    case '_':
        modelLoader.setScale(std::max(0.1f, modelLoader.getScale() * 0.9f));             // Decrease scale by 10%, but not below 0.1
        break;
    case 'g':                                                                            // Toggle grid visibility
    case 'G':
        renderer.toggleGrid();                                                           // Toggle grid on/off
        break;
    case 'c':                                                                            // Reset camera position
    case 'C':
        camera.reset();                                                                  // Reset camera to default position
        break;
    case 'm':                                                                            // Reset model transformations
    case 'M':
        modelLoader.resetModel();                                                        // Reset model position, rotation, and scale
        break;
    case 'n':                                                                            // Load a new model
    case 'N':
        modelLoader.loadNewModel();                                                      // Prompt user for new model file
        break;
    case 27:                                                                             // ESC key
        exit(0);                                                                         // Exit the application
        break;
    case 'b':                                                                            // Toggle debug rendering mode
    case 'B':
        modelLoader.cycleDebugMode();                                                    // Cycle through debug modes
        printf("Debug mode: %d\n", modelLoader.getDebugMode());                          // Print current debug mode
        break;
        
    }
    glutPostRedisplay();                                                                 // Request display update
}

// Special keys callback function (arrow keys, function keys, etc.)
void InputHandler::specialKeysCallback(int key, int x, int y) {
    // Handle special keys if needed
    glutPostRedisplay();                                                                 // Request display update
}

// Mouse button callback function
void InputHandler::mouseButtonCallback(int button, int state, int x, int y) {
    lastMouseX = x;                                                                      // Store current mouse X position
    lastMouseY = y;                                                                      // Store current mouse Y position

    // Update mouse button states
    if (button == GLUT_LEFT_BUTTON) {
        mouseLeftDown = (state == GLUT_DOWN);                                            // Update left button state
    }
    else if (button == GLUT_RIGHT_BUTTON) {
        mouseRightDown = (state == GLUT_DOWN);                                           // Update right button state
    }
}

// Mouse motion callback function
void InputHandler::mouseMotionCallback(int x, int y) {
    int deltaX = x - lastMouseX;                                                         // Calculate X movement
    int deltaY = y - lastMouseY;                                                         // Calculate Y movement

    // Update camera orientation based on mouse movement
    if (mouseLeftDown) {
        // Invert the deltaX and deltaY to fix the reversed controls
        camera.updateOrientation(-deltaX, -deltaY);                                      // Use the camera's orientation update method with inverted deltas
    }

    lastMouseX = x;                                                                      // Update last mouse X position
    lastMouseY = y;                                                                      // Update last mouse Y position

    glutPostRedisplay();                                                                 // Request display update
}

// Menu callback function
void InputHandler::menuCallback(int option) {
    switch (option) {
    case MENU_LOAD_MODEL:
        modelLoader.loadNewModel();
        break;
    case MENU_RESET_CAMERA:
        camera.reset();
        break;
    case MENU_RESET_MODEL:
        modelLoader.resetModel();
        break;
    case MENU_TOGGLE_GRID:
        renderer.toggleGrid();
        break;
    case MENU_EXIT:
        exit(0);
        break;
    }
    glutPostRedisplay();
}

// Create the right-click context menu
void InputHandler::createMenu() {
    int menu = glutCreateMenu(::menuCallback);
    glutAddMenuEntry("Load New Model", MENU_LOAD_MODEL);
    glutAddMenuEntry("Reset Camera", MENU_RESET_CAMERA);
    glutAddMenuEntry("Reset Model", MENU_RESET_MODEL);
    glutAddMenuEntry("Toggle Grid", MENU_TOGGLE_GRID);
    glutAddMenuEntry("Exit", MENU_EXIT);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

// Static wrapper functions for GLUT callbacks
void keyboardWrapper(unsigned char key, int x, int y) {
    inputHandler.keyboardCallback(key, x, y);
}

void specialKeysWrapper(int key, int x, int y) {
    inputHandler.specialKeysCallback(key, x, y);
}

void mouseButtonWrapper(int button, int state, int x, int y) {
    inputHandler.mouseButtonCallback(button, state, x, y);
}

void mouseMotionWrapper(int x, int y) {
    inputHandler.mouseMotionCallback(x, y);
}

void menuCallback(int option) {
    inputHandler.menuCallback(option);
}