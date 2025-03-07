#pragma once
#include <freeglut.h>

// Forward declarations
void resetCamera();
void resetModel();
void loadNewModel();
void toggleGrid();

// Menu option identifiers
enum MenuOptions {
    MENU_LOAD_MODEL = 1,                               // Option to load a new model
    MENU_RESET_CAMERA,                                 // Option to reset camera position
    MENU_RESET_MODEL,                                  // Option to reset model transformations
    MENU_TOGGLE_GRID,                                  // Option to toggle grid visibility
    MENU_EXIT                                          // Option to exit the application
};

class InputHandler {
public:
    // Initialize input handling
    static void init();

    // Callback functions for GLUT
    static void keyboardCallback(unsigned char key, int x, int y);
    static void specialKeysCallback(int key, int x, int y);
    static void mouseButtonCallback(int button, int state, int x, int y);
    static void mouseMotionCallback(int x, int y);
    static void menuCallback(int option);

    // Create the right-click context menu
    static void createMenu();
};