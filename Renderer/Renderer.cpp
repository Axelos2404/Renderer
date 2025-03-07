#include "Renderer.h"
#include "Camera.h"
#include "ModelLoader.h"
#include <cmath>

// Define PI constant if not already defined by the compiler
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Window configuration
int windowWidth = 800;                                                                   // Initial window width in pixels
int windowHeight = 600;                                                                  // Initial window height in pixels

// Configuration flag for grid visibility
bool showGrid = true;                                                                    // Controls whether the reference grid is displayed

// Toggle grid visibility
void toggleGrid() {
    showGrid = !showGrid;                                                                // Invert grid visibility flag
}

// Display callback function - called whenever the window needs to be redrawn
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);                                  // Clear color and depth buffers
    glLoadIdentity();                                                                    // Reset the modelview matrix

    // Set up the camera view
    setupCamera();

    // Draw the reference grid if enabled in settings
    if (showGrid) {
        drawWireGrid(20.0f, 20, -2.0f);                                                  // Draw a 20x20 grid at y=-2.0 (below the model)
    }

    // Draw the 3D model with current transformations
    drawModel();

    // Save the current projection matrix for 2D overlay drawing
    glMatrixMode(GL_PROJECTION);                                                         // Switch to projection matrix mode
    glPushMatrix();                                                                      // Save current projection matrix
    glLoadIdentity();                                                                    // Reset projection matrix
    glOrtho(0, windowWidth, 0, windowHeight, -1, 1);                                     // Set orthographic projection for 2D overlay

    // Save the current modelview matrix for 2D overlay drawing
    glMatrixMode(GL_MODELVIEW);                                                          // Switch to modelview matrix mode
    glPushMatrix();                                                                      // Save current modelview matrix
    glLoadIdentity();                                                                    // Reset modelview matrix

    // Disable depth testing and lighting for 2D overlay elements
    glDisable(GL_DEPTH_TEST);                                                            // Disable depth testing for 2D elements
    glDisable(GL_LIGHTING);                                                              // Disable lighting for 2D elements

    // Draw coordinate axes indicator in bottom-left corner of the screen
    const float axisSize = 60.0f;                                                        // Size of the axis indicator in pixels
    const float margin = 10.0f;                                                          // Margin from screen edge in pixels
    const float centerX = margin + axisSize * 0.5f;                                      // X center of the axis indicator
    const float centerY = margin + axisSize * 0.5f;                                      // Y center of the axis indicator

    // Calculate rotation matrix based on camera orientation to align axes with camera view
    float rotMatrix[9];                                                                  // 3x3 rotation matrix for axis orientation

    // Forward vector (Z-axis) - direction the camera is looking
    rotMatrix[6] = sin(cameraYaw * M_PI / 180.0f) * cos(cameraPitch * M_PI / 180.0f);   // Z-axis X component
    rotMatrix[7] = sin(cameraPitch * M_PI / 180.0f);                                     // Z-axis Y component
    rotMatrix[8] = cos(cameraYaw * M_PI / 180.0f) * cos(cameraPitch * M_PI / 180.0f);   // Z-axis Z component

    // Right vector (X-axis) - perpendicular to forward and up vectors
    rotMatrix[0] = sin((cameraYaw + 90.0f) * M_PI / 180.0f);                            // X-axis X component
    rotMatrix[1] = 0.0f;                                                                 // X-axis Y component
    rotMatrix[2] = cos((cameraYaw + 90.0f) * M_PI / 180.0f);                            // X-axis Z component

    // Up vector (Y-axis) - cross product of forward and right vectors
    rotMatrix[3] = -rotMatrix[6] * rotMatrix[1] + rotMatrix[7] * rotMatrix[0];          // Y-axis X component
    rotMatrix[4] = -rotMatrix[8] * rotMatrix[0] + rotMatrix[6] * rotMatrix[2];          // Y-axis Y component
    rotMatrix[5] = -rotMatrix[7] * rotMatrix[2] + rotMatrix[8] * rotMatrix[1];          // Y-axis Z component

    // Normalize the up vector to ensure unit length
    float upLength = sqrt(rotMatrix[3] * rotMatrix[3] + rotMatrix[4] * rotMatrix[4] + rotMatrix[5] * rotMatrix[5]);
    if (upLength > 0.0001f) {                                                            // Avoid division by zero
        rotMatrix[3] /= upLength;                                                        // Normalize X component
        rotMatrix[4] /= upLength;                                                        // Normalize Y component
        rotMatrix[5] /= upLength;                                                        // Normalize Z component
    }

    // Draw the three coordinate axes as colored lines
    glBegin(GL_LINES);                                                                   // Begin drawing lines

    // X axis (red)
    glColor3f(1.0f, 0.0f, 0.0f);                                                         // Set color to red for X axis
    glVertex3f(centerX, centerY, 0.0f);                                                  // Start at center point
    glVertex3f(centerX + rotMatrix[0] * axisSize * 0.5f,                                 // End at X axis tip
        centerY + rotMatrix[1] * axisSize * 0.5f, 0.0f);

    // Y axis (green) - Invert Y component for screen space (screen Y is inverted)
    glColor3f(0.0f, 1.0f, 0.0f);                                                         // Set color to green for Y axis
    glVertex3f(centerX, centerY, 0.0f);                                                  // Start at center point
    glVertex3f(centerX + rotMatrix[3] * axisSize * 0.5f,                                 // End at Y axis tip
        centerY + rotMatrix[4] * axisSize * 0.5f * -1.0f, 0.0f);                         // Invert Y for screen space

    // Z axis (blue)
    glColor3f(0.0f, 0.0f, 1.0f);                                                         // Set color to blue for Z axis
    glVertex3f(centerX, centerY, 0.0f);                                                  // Start at center point
    glVertex3f(centerX + rotMatrix[6] * axisSize * 0.5f,                                 // End at Z axis tip
        centerY + rotMatrix[7] * axisSize * 0.5f, 0.0f);
    glEnd();                                                                             // End line drawing

    // Reset color to white for subsequent rendering
    glColor3f(1.0f, 1.0f, 1.0f);                                                         // Reset color to white

    // Restore previous OpenGL states and matrices
    glEnable(GL_DEPTH_TEST);                                                             // Re-enable depth testing
    glEnable(GL_LIGHTING);                                                               // Re-enable lighting

    // Restore saved projection and modelview matrices
    glMatrixMode(GL_PROJECTION);                                                         // Switch to projection matrix mode
    glPopMatrix();                                                                       // Restore saved projection matrix
    glMatrixMode(GL_MODELVIEW);                                                          // Switch to modelview matrix mode
    glPopMatrix();                                                                       // Restore saved modelview matrix

    glutSwapBuffers();                                                                   // Swap front and back buffers to display the rendered scene
}

// Reshape callback function - called when window is resized
void reshape(int width, int height) {
    windowWidth = width;                                                                 // Store new window width
    windowHeight = height;                                                               // Store new window height

    glViewport(0, 0, width, height);                                                     // Set viewport to cover entire window
    glMatrixMode(GL_PROJECTION);                                                         // Switch to projection matrix mode
    glLoadIdentity();                                                                    // Reset projection matrix
    gluPerspective(45.0f,                                                                // Field of view angle (45 degrees)
        (float)width / (float)height,                                                    // Aspect ratio
        0.1f,                                                                            // Near clipping plane
        100.0f);                                                                         // Far clipping plane
    glMatrixMode(GL_MODELVIEW);                                                          // Switch back to modelview matrix mode
}

// Setup lighting parameters for the 3D scene
void setupLighting() {
    // Define light properties
    GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };                                 // Ambient light color (dim white)
    GLfloat diffuseLight[] = { 0.8f, 0.8f, 0.8f, 1.0f };                                 // Diffuse light color (bright white)
    GLfloat specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };                                // Specular light color (pure white)
    GLfloat position[] = { 1.0f, 1.0f, 1.0f, 0.0f };                                     // Light position (directional light)

    // Configure light 0
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);                                      // Set ambient light properties
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);                                      // Set diffuse light properties
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);                                    // Set specular light properties
    glLightfv(GL_LIGHT0, GL_POSITION, position);                                         // Set light position

    // Define material properties
    GLfloat materialAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };                              // Material ambient color
    GLfloat materialDiffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };                              // Material diffuse color
    GLfloat materialSpecular[] = { 0.5f, 0.5f, 0.5f, 1.0f };                             // Material specular color
    GLfloat materialShininess = 50.0f;                                                   // Material shininess (0-128)

    // Configure material properties
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, materialAmbient);                        // Set material ambient properties
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDiffuse);                        // Set material diffuse properties
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialSpecular);                      // Set material specular properties
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, materialShininess);                     // Set material shininess

    // Enable lighting and light 0
    glEnable(GL_LIGHTING);                                                               // Enable lighting calculations
    glEnable(GL_LIGHT0);                                                                 // Enable light 0
    glEnable(GL_COLOR_MATERIAL);                                                         // Enable material color tracking
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);                          // Material tracks glColor
}