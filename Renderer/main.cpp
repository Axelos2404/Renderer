// Include necessary libraries for OpenGL, file I/O, and data structures
#include <freeglut.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <vector>
#include "Renderer.h"
#include "Camera.h"
#include "ModelLoader.h"
#include "InputHandler.h"

// Define PI constant if not already defined by the compiler
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Function prototypes for GLUT callbacks
void init();
void createMenu();
void keyboard(unsigned char key, int x, int y);
void mouseButton(int button, int state, int x, int y);
void mouseMotion(int x, int y);
void menuCallback(int option);

// Main entry point for the application
int main(int argc, char** argv) {
    // Initialize GLUT with command line parameters
    glutInit(&argc, argv);

    // Set up display mode with double buffering, RGBA color, and depth buffer
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    // Set initial window size
    glutInitWindowSize(windowWidth, windowHeight);

    // Set initial window position on screen
    glutInitWindowPosition(100, 100);

    // Create window with title
    glutCreateWindow("3D Model Viewer");

    // Initialize OpenGL settings and load default model
    init();

    // Register callback functions for various events
    glutDisplayFunc(display);           // Display callback for rendering
    glutReshapeFunc(reshape);           // Reshape callback for window resizing
    glutKeyboardFunc(keyboard);         // Keyboard callback for key presses
    glutMouseFunc(mouseButton);         // Mouse button callback for clicks
    glutMotionFunc(mouseMotion);        // Mouse motion callback for dragging

    // Create right-click context menu
    createMenu();

    // Display controls information for user reference
    printf("\n3D OBJ Viewer Controls:\n");                                               // Print header for controls info
    printf("Camera Controls:\n");                                                        // Print camera controls section
    printf("  W, A, S, D: Move camera forward, left, backward, right\n");                // WASD movement controls
    printf("  Q, E: Move camera up, down\n");                                            // Vertical camera movement
    printf("  Left Mouse Button + Drag: Look around\n");                                 // Mouse look controls
    printf("  R: Reset camera position\n\n");                                            // Camera reset control

    printf("Model Controls:\n");                                                         // Print model controls section
    printf("  I, J, K, L: Move model forward, left, backward, right\n");                 // IJKL movement controls
    printf("  U, O: Move model up, down\n");                                             // Vertical model movement
    printf("  X, Y, Z: Rotate model around X, Y, Z axes (hold Shift for opposite direction)\n"); // Rotation controls
    printf("  +, -: Scale model up/down\n");                                             // Scaling controls
    printf("  r: Reset model position and rotation\n\n");                                // Model reset control

    printf("Other Controls:\n");                                                         // Print other controls section
    printf("  Right-click: Open menu\n");                                                // Menu access
    printf("  ESC: Exit the application\n\n");                                           // Exit control


    // Enter the GLUT event processing loop
    glutMainLoop();

    return 0;
}

// Initialize OpenGL settings and load default model
void init() {
    // Set clear color to dark gray
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    // Enable depth testing for proper 3D rendering
    glEnable(GL_DEPTH_TEST);

    // Enable smooth shading
    glShadeModel(GL_SMOOTH);

    // Setup lighting parameters
    setupLighting();

    // Initialize camera to default position
    resetCamera();

    // Try to load a default model if available
    // if (!loadOBJ("models/cube.obj")) {
    //    printf("Failed to load default model. Please load a model using the menu.\n");
    //}

    // Set up initial projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}