#pragma once
#include <freeglut.h>
#include "Camera.h"
#include "ModelLoader.h"
#include "Renderer.h"

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
    // Constructor
    InputHandler(Camera& cam, ModelLoader& model, Renderer& rend);

    // Initialize input handling
    void init();

    // Callback functions for GLUT
    void keyboardCallback(unsigned char key, int x, int y);
    void specialKeysCallback(int key, int x, int y);
    void mouseButtonCallback(int button, int state, int x, int y);
    void mouseMotionCallback(int x, int y);
    void menuCallback(int option);

    // Create the right-click context menu
    void createMenu();

private:
    // References to other components
    Camera& camera;
    ModelLoader& modelLoader;
    Renderer& renderer;
    
    // Mouse tracking variables
    int lastMouseX;
    int lastMouseY;
    bool mouseLeftDown;
    bool mouseRightDown;
};

// Global input handler instance
extern InputHandler inputHandler;

// Static wrapper functions for GLUT callbacks
void keyboardWrapper(unsigned char key, int x, int y);
void specialKeysWrapper(int key, int x, int y);
void mouseButtonWrapper(int button, int state, int x, int y);
void mouseMotionWrapper(int x, int y);
void menuCallback(int option);