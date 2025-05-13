#pragma once
#include <freeglut.h>
#include "Camera.h"  // Include Camera header
#include "ModelLoader.h"

class Renderer {
public:
    // Constructor
    Renderer(int width = 800, int height = 600);
    
    // Initialize renderer
    void init();
    
    // Display callback function
    void display();
    
    // Reshape callback function
    void reshape(int width, int height);
    
    // Setup lighting parameters
    void setupLighting();
    
    // Toggle grid visibility
    void toggleGrid();
    
    // Add getter methods for width and height
    int getWidth() const { return width; }
    int getHeight() const { return height; }

    // Set references to other components
    void setComponents(Camera* cam, ModelLoader* model) {
        cameraPtr = cam;
        modelLoaderPtr = model;
    }
    
private:
    // Window configuration
    int width;
    int height;
    
    // Configuration flag for grid visibility
    bool showGrid;
    
    // Reference to the global camera instance
    // Use pointers instead of references
    Camera* cameraPtr;
    ModelLoader* modelLoaderPtr;
};

// Global renderer instance declaration
extern Renderer renderer;

// Wrapper functions for GLUT callbacks
void displayWrapper();
void reshapeWrapper(int width, int height);