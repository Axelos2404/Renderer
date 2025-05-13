#pragma once
#include <vector>
#include <string>
#include <freeglut.h>
#include <map>
#include <iostream>

// Data structures for 3D model components
struct Vertex {
    float x, y, z;
};

struct TextureCoord {
    float u, v;
};

struct Normal {
    float x, y, z;
};

struct Face {
    int vertexIndices[4];
    int textureIndices[4];
    int normalIndices[4];
    int vertexCount;
};

struct Material {
    std::string name;
    float ambient[4] = {0.2f, 0.2f, 0.2f, 1.0f};
    float diffuse[4] = {0.8f, 0.8f, 0.8f, 1.0f};
    float specular[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    float shininess = 0.0f;
    GLuint textureID = 0;
    std::string texturePath;
    bool hasTexture = false;
};

// Debug rendering modes
enum DebugRenderMode {
    DEBUG_NONE = 0,           // Normal rendering
    DEBUG_SOLID_COLOR,        // Solid color rendering
    DEBUG_CHECKERBOARD,       // Force checkerboard
    DEBUG_WIREFRAME,          // Wireframe rendering
    DEBUG_NORMALS,            // Show normals
    DEBUG_TEXCOORDS           // Show texture coordinates
};

class ModelLoader {
public:
    // Constructor
    ModelLoader();

    // Destructor
    ~ModelLoader();
    
    // Model loading and manipulation
    bool loadOBJ(const char* filename);
    bool loadMTL(const char* filename);  // Add this line
    void resetModel();
    void loadNewModel();
    
    // Rendering functions
    void drawModel();
    void drawWireGrid(float size, int divisions, float y);
    
    // Helper methods for parsing OBJ file
    void parseFace(const char* line);
    bool tryParseQuadWithTexAndNorm(const char* line, Face& face);
    bool tryParseTriWithTexAndNorm(const char* line, Face& face);
    bool tryParseQuadWithNorm(const char* line, Face& face);
    bool tryParseTriWithNorm(const char* line, Face& face);
    bool tryParseQuadVerticesOnly(const char* line, Face& face);
    bool tryParseTriVerticesOnly(const char* line, Face& face);
    
    // Getters and setters for model transformations
    float getX() const { return modelX; }
    float getY() const { return modelY; }
    float getZ() const { return modelZ; }
    float getRotX() const { return modelRotX; }
    float getRotY() const { return modelRotY; }
    float getRotZ() const { return modelRotZ; }
    float getScale() const { return modelScale; }
    
    void setX(float x) { modelX = x; }
    void setY(float y) { modelY = y; }
    void setZ(float z) { modelZ = z; }
    void setRotX(float rx) { modelRotX = rx; }
    void setRotY(float ry) { modelRotY = ry; }
    void setRotZ(float rz) { modelRotZ = rz; }
    void setScale(float s) { modelScale = s; }

    // Debug mode functions
    void setDebugMode(DebugRenderMode mode) { debugMode = mode; }
    DebugRenderMode getDebugMode() const { return debugMode; }
    void cycleDebugMode();
    
private:
    // Model data containers
    std::vector<Vertex> vertices;
    std::vector<TextureCoord> textureCoords;
    std::vector<Normal> normals;
    std::vector<Face> faces;
    std::string mtlLibPath;
    std::vector<Material> materials;
    std::map<std::string, int> materialMap;
    std::vector<int> faceMaterials;
    
    // Model transformation variables
    float modelX, modelY, modelZ;
    float modelRotX, modelRotY, modelRotZ;
    float modelScale;

    // Helper methods for texture loading
    GLuint loadTexture(const char* filename);
    void parseMTLLine(const char* line, Material& currentMaterial);

    // Create a checkerboard pattern as placeholder texture
    void createCheckerboardTexture(GLuint textureID);
    
    // Debug rendering mode
    DebugRenderMode debugMode = DEBUG_NONE;
    GLuint debugCheckerboardTextureID = 0;  // Store the debug texture ID
};

// Global model loader instance
extern ModelLoader modelLoader;

