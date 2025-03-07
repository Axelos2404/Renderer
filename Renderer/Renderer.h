#pragma once
#include <freeglut.h>

// Window configuration
extern int windowWidth;                                                                  // Window width in pixels
extern int windowHeight;                                                                 // Window height in pixels

// Configuration flag for grid visibility
extern bool showGrid;                                                                    // Controls whether the reference grid is displayed

// Function declarations
void display();                                                                          // Display callback function
void reshape(int width, int height);                                                     // Reshape callback function
void setupLighting();                                                                    // Setup lighting parameters
void toggleGrid();                                                                       // Toggle grid visibility