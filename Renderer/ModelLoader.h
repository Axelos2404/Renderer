#pragma once
#include <vector>
#include <string>
#if _WIN64
#include <fbxsdk.h>
#endif

// Structures for 3D model data
struct Vertex {
    float x, y, z;                                                                       // 3D position coordinates
};

struct TextureCoord {
    float u, v;                                                                          // 2D texture coordinates
};

struct Normal {
    float x, y, z;                                                                       // Normal vector components
};

struct Face {
    int vertexIndices[4];                                                                // Indices of vertices (up to 4 for quads)
    int textureIndices[4];                                                               // Indices of texture coordinates
    int normalIndices[4];                                                                // Indices of normal vectors
    int vertexCount;                                                                     // Number of vertices (3 for triangle, 4 for quad)
};

// Model data containers
extern std::vector<Vertex> vertices;                                                     // Collection of vertices
extern std::vector<TextureCoord> textureCoords;                                          // Collection of texture coordinates
extern std::vector<Normal> normals;                                                      // Collection of normal vectors
extern std::vector<Face> faces;                                                          // Collection of faces

// Model transformation variables
extern float modelX, modelY, modelZ;                                                     // Model position
extern float modelRotX, modelRotY, modelRotZ;                                            // Model rotation angles
extern float modelScale;                                                                 // Model scale factor

// Function declarations
bool loadOBJ(const char* filename);                                                      // Load OBJ file
bool loadFBX(const char* filename);                                                      // Load FBX file
void loadNewModel();                                                                     // Load a new model from user input
void resetModel();                                                                       // Reset model transformations
void drawModel();                                                                        // Render the model
void drawWireGrid(float size, int divisions, float y);                                   // Draw a reference grid on the XZ plane

// Helper function for FBX loading
#if _WIN64
void ProcessFbxNode(FbxNode* node);
#endif
