#include <freeglut.h>
#include "Camera.h"
#include "ModelLoader.h"
#include "Renderer.h"
#include "InputHandler.h"

// Global instances
Camera camera;
ModelLoader modelLoader;
Renderer renderer(800, 600);
InputHandler inputHandler(camera, modelLoader, renderer);

// Initialize OpenGL settings and load default model
void init() {
    // Initialize renderer
    renderer.init();
    
    // Load default model (cube.obj)
    if (!modelLoader.loadOBJ("models/cube.obj")) {
        printf("Failed to load default model. Using empty scene.\n");
    }
    
    // Initialize input handling
    inputHandler.init();

    // Display controls tutorial in console
    printf("\n=== 3D Model Viewer Controls ===\n");
    printf("Camera Movement:\n");
    printf("  W/S - Move forward/backward\n");
    printf("  A/D - Move left/right\n");
    printf("  Q/E - Move up/down\n");
    printf("  Mouse Drag - Look around\n");
    printf("\nModel Manipulation:\n");
    printf("  I/K - Move model forward/backward\n");
    printf("  J/L - Move model left/right\n");
    printf("  U/O - Move model up/down\n");
    printf("  R/F/V - Rotate model around X/Y/Z axis\n");
    printf("  +/- - Scale model up/down\n");
    printf("\nDebugging Controls:\n");
    printf("  B - Cycle through debug rendering modes\n");
    printf("      (Normal -> Solid Color -> Wireframe -> Normals -> Texture Coords)\n");
    
    printf("\nOther Controls:\n");
    printf("  G - Toggle grid visibility\n");
    printf("  C - Reset camera position\n");
    printf("  M - Reset model transformations\n");
    printf("  N - Load a new model\n");
    printf("  ESC - Exit application\n");
    printf("  Right-click - Open context menu\n");
    printf("==============================\n\n");
}

int main(int argc, char** argv) {
    // Initialize GLUT with command line parameters
    glutInit(&argc, argv);

    // Set up display mode with double buffering, RGBA color, and depth buffer
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    // Set initial window size
    glutInitWindowSize(renderer.getWidth(), renderer.getHeight());

    // Set initial window position on screen
    glutInitWindowPosition(100, 100);

    // Create window with title
    glutCreateWindow("3D Model Viewer");

    // Set component references after all objects are created
    renderer.setComponents(&camera, &modelLoader);
    
    // Initialize OpenGL settings and load default model
    init();

    // Register callback functions for various events
    glutDisplayFunc(displayWrapper);           // Display callback for rendering
    glutReshapeFunc(reshapeWrapper);           // Reshape callback for window resizing

    // Start the main GLUT event processing loop
    glutMainLoop();

    return 0;
}