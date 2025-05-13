#include "Renderer.h"
#include "Camera.h"
#include "ModelLoader.h"
#include <cmath>

// Define PI constant
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Constructor implementation
Renderer::Renderer(int width, int height) {
    this->width = width;
    this->height = height;
    showGrid = true;
}

// Initialize renderer
void Renderer::init() {
    // Set up OpenGL state
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);                                                             // Set depth function
    glShadeModel(GL_SMOOTH);                                                            // Enable smooth shading
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);                                  // Set perspective correction hint
    
    // Set up lighting
    setupLighting();
}

// Toggle grid visibility
void Renderer::toggleGrid() {
    showGrid = !showGrid;                                                                // Invert grid visibility flag
}

// Display callback function - called whenever the window needs to be redrawn
void Renderer::display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);                                  // Clear color and depth buffers
    glLoadIdentity();                                                                    // Reset the modelview matrix

    // Set up the camera view
    if (cameraPtr) {
        cameraPtr->setupView();
    }
    
    // Draw the reference grid if enabled in settings
    if (showGrid && modelLoaderPtr) {
        modelLoaderPtr->drawWireGrid(20.0f, 20, -2.0f);
    }
    
    // Draw the 3D model with current transformations
    if (modelLoaderPtr) {
        modelLoaderPtr->drawModel();
    }

    // Save the current projection matrix for 2D overlay drawing
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1);                                                 // Set orthographic projection for 2D overlay

    // Save the current modelview matrix for 2D overlay drawing
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Disable depth testing and lighting for 2D overlay elements
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    // Draw coordinate axes indicator in bottom-left corner of the screen
    const float axisSize = 60.0f;                                                        // Size of the axis indicator in pixels
    const float margin = 10.0f;                                                          // Margin from screen edge in pixels
    const float centerX = margin + axisSize * 0.5f;                                      // X center of the axis indicator
    const float centerY = margin + axisSize * 0.5f;                                      // Y center of the axis indicator

    // Get camera yaw and pitch in radians for rotation calculations
    float yawRad = cameraPtr->getYaw() * M_PI / 180.0f;
    float pitchRad = cameraPtr->getPitch() * M_PI / 180.0f;

    // Calculate rotation matrix based on camera orientation
    float rotMatrix[9];  // 3x3 rotation matrix

    // Z-axis (forward) vector calculation
    rotMatrix[6] = sin(yawRad) * cos(pitchRad);                                           // Z-axis X component
    rotMatrix[7] = -sin(pitchRad);                                                        // Z-axis Y component (negative because pitch up means looking down)
    rotMatrix[8] = cos(yawRad) * cos(pitchRad);                                           // Z-axis Z component

    // Normalize Z-axis vector
    float zLength = sqrt(rotMatrix[6] * rotMatrix[6] + rotMatrix[7] * rotMatrix[7] + rotMatrix[8] * rotMatrix[8]);
    if (zLength > 0.0001f) {
        rotMatrix[6] /= zLength;
        rotMatrix[7] /= zLength;
        rotMatrix[8] /= zLength;
    }

    // X-axis (right) vector calculation - using cross product of world up (0,1,0) and forward
    rotMatrix[0] = sin(yawRad + M_PI / 2.0f);                                             // X-axis X component (90 degrees right of forward)
    rotMatrix[1] = 0.0f;                                                                  // X-axis Y component
    rotMatrix[2] = cos(yawRad + M_PI / 2.0f);                                             // X-axis Z component

    // Normalize X-axis vector
    float xLength = sqrt(rotMatrix[0] * rotMatrix[0] + rotMatrix[1] * rotMatrix[1] + rotMatrix[2] * rotMatrix[2]);
    if (xLength > 0.0001f) {
        rotMatrix[0] /= xLength;
        rotMatrix[1] /= xLength;
        rotMatrix[2] /= xLength;
    }

    // Y-axis (up) vector calculation - using cross product of Z and X
    // Cross product: Y = Z × X
    rotMatrix[3] = rotMatrix[7] * rotMatrix[2] - rotMatrix[8] * rotMatrix[1];             // Y-axis X component
    rotMatrix[4] = rotMatrix[8] * rotMatrix[0] - rotMatrix[6] * rotMatrix[2];             // Y-axis Y component
    rotMatrix[5] = rotMatrix[6] * rotMatrix[1] - rotMatrix[7] * rotMatrix[0];             // Y-axis Z component

    // Normalize Y-axis vector
    float yLength = sqrt(rotMatrix[3] * rotMatrix[3] + rotMatrix[4] * rotMatrix[4] + rotMatrix[5] * rotMatrix[5]);
    if (yLength > 0.0001f) {
        rotMatrix[3] /= yLength;
        rotMatrix[4] /= yLength;
        rotMatrix[5] /= yLength;
    }

    // Draw the axes with colors
    const float axisLength = axisSize * 0.4f;                                           // Length of each axis line

    // Draw X-axis (Red)
    glLineWidth(2.0f);                                                                  // Set line width for axes
    glBegin(GL_LINES);
    // X-axis (Red)
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f(centerX, centerY);                                                       // Origin point
    glVertex2f(centerX + rotMatrix[0] * axisLength,                                     // Endpoint using X component of X-axis
        centerY + rotMatrix[1] * axisLength);                                           // Endpoint using Y component of X-axis

    // Y-axis (Green)
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex2f(centerX, centerY);                                                       // Origin point
    glVertex2f(centerX + rotMatrix[3] * axisLength,
        centerY + rotMatrix[4] * axisLength);                                           // Endpoint using Y component of Y-axis

    // Z-axis (Blue)
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex2f(centerX, centerY);                                                       // Origin point
    glVertex2f(centerX + rotMatrix[6] * axisLength,                                     // Endpoint using X component of Z-axis
        centerY + rotMatrix[7] * axisLength);                                           // Endpoint using Y component of Z-axis
    glEnd();

    // Draw axis labels (X, Y, Z)
    const float textOffset = axisLength + 5.0f;                                          // Offset for text labels from axis ends

    // Render the axis labels using bitmap fonts
    glColor3f(1.0f, 0.0f, 0.0f);
    glRasterPos2f(centerX + rotMatrix[0] * textOffset,                                   // X position for X label
        centerY + rotMatrix[1] * textOffset);                                            // Y position for X label
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'X');                                  // Draw 'X' character

    glColor3f(0.0f, 1.0f, 0.0f);
    glRasterPos2f(centerX + rotMatrix[3] * textOffset,                                   // X position for Y label
        centerY + rotMatrix[4] * textOffset);                                            // Y position for Y label
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'Y');                                  // Draw 'Y' character

    glColor3f(0.0f, 0.0f, 1.0f);
    glRasterPos2f(centerX + rotMatrix[6] * textOffset,                                   // X position for Z label
        centerY + rotMatrix[7] * textOffset);                                            // Y position for Z label
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'Z');                                  // Draw 'Z' character

    // Reset color to white for subsequent rendering
    glColor3f(1.0f, 1.0f, 1.0f);                                                         // Reset color to white

    // Restore previous OpenGL states and matrices
    glEnable(GL_DEPTH_TEST);                                                             // Re-enable depth testing
    glEnable(GL_LIGHTING);                                                               // Re-enable lighting

    // Restore saved projection and modelview matrices
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW); 
    glPopMatrix();

    glutSwapBuffers();                                                                   // Swap front and back buffers to display the rendered scene
}

// Reshape callback function - called when window is resized
void Renderer::reshape(int width, int height) {
    this->width = width;
    this->height = height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f,                                                                // Field of view angle (45 degrees)
        (float)width / (float)height,                                                    // Aspect ratio
        0.1f,                                                                            // Near clipping plane
        100.0f);                                                                         // Far clipping plane
    glMatrixMode(GL_MODELVIEW);
}

// Setup lighting parameters for the 3D scene
void Renderer::setupLighting() {
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

    //// Define material properties
    GLfloat materialAmbient[] = { 1.0f, 1.0f, 1.0f, 1.0f };  // Brighter ambient
    GLfloat materialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };  // Full diffuse
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

// Wrapper functions for GLUT callbacks
void displayWrapper() {
    renderer.display();
}

void reshapeWrapper(int width, int height) {
    renderer.reshape(width, height);
}