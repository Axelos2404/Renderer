#include "ModelLoader.h"
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <freeglut.h>
#include <map>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Constructor
ModelLoader::ModelLoader() {
    // Initialize with dummy values at index 0
    vertices = { {0, 0, 0} };
    textureCoords = { {0, 0} };
    normals = { {0, 0, 0} };

    // Initialize transformation variables
    modelX = 0.0f;
    modelY = 0.0f;
    modelZ = 0.0f;
    modelRotX = 0.0f;
    modelRotY = 0.0f;
    modelRotZ = 0.0f;
    modelScale = 1.0f;
}


ModelLoader::~ModelLoader()
{
    // Clean up texture resources
    for (auto& material : materials) {
        if (material.hasTexture && material.textureID != 0) {
            glDeleteTextures(1, &material.textureID);
        }
    }
    
    // Clean up debug texture
    if (debugCheckerboardTextureID != 0) {
        glDeleteTextures(1, &debugCheckerboardTextureID);
    }
}

// Reset model transformations to default values
void ModelLoader::resetModel() {
    modelX = 0.0f;                                                                       // Reset X position
    modelY = 0.0f;                                                                       // Reset Y position
    modelZ = 0.0f;                                                                       // Reset Z position
    modelRotX = 0.0f;                                                                    // Reset X rotation
    modelRotY = 0.0f;                                                                    // Reset Y rotation
    modelRotZ = 0.0f;                                                                    // Reset Z rotation
    modelScale = 1.0f;                                                                   // Reset scale
}

// Load OBJ file and parse its contents
bool ModelLoader::loadOBJ(const char* filename) {
    FILE* file;
    errno_t err = fopen_s(&file, filename, "r");
    if (err != 0 || file == NULL) {
        printf("Error opening file: %s\n", filename);
        return false;                                                                    
    }

    // Clear previous model data
    vertices = { {0, 0, 0} };
    textureCoords = { {0, 0} };
    normals = { {0, 0, 0} };
    faces.clear();
    materials.clear();
    materialMap.clear();
    faceMaterials.clear();
    mtlLibPath = "";

    // Reset model transformations
    resetModel();                                                                        // Reset position, rotation, and scale

    char line[256];                                                                      // Buffer for reading lines from file
    std::string currentMaterial = "";                                                    // Current material name

    // Extract directory path from the OBJ filename
    std::string objPath = filename;
    size_t lastSlash = objPath.find_last_of("/\\");
    std::string directory = (lastSlash != std::string::npos) ? objPath.substr(0, lastSlash + 1) : "";

    while (fgets(line, sizeof(line), file)) {                                            // Read file line by line
        // Process vertex data
        if (strncmp(line, "v ", 2) == 0) {                                               // Line defines a vertex
            Vertex vertex;                                                               // Create new vertex
            sscanf_s(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);               // Analyes vertex coordinates
            vertices.push_back(vertex);                                                  // Add vertex to collection
        }
        else if (strncmp(line, "vt ", 3) == 0) {                                         // Line defines a texture coordinate
            TextureCoord texCoord;                                                       // Create new texture coordinate
            sscanf_s(line, "vt %f %f", &texCoord.u, &texCoord.v);                        // Analyse texture coordinates
            textureCoords.push_back(texCoord);                                           // Add texture coordinate to collection
        }
        else if (strncmp(line, "vn ", 3) == 0) {                                         // Line defines a normal vector
            Normal normal;                                                               // Create new normal
            sscanf_s(line, "vn %f %f %f", &normal.x, &normal.y, &normal.z);              // Analyse normal vector
            normals.push_back(normal);                                                   // Add normal to collection
        }
        else if (strncmp(line, "f ", 2) == 0) {                                          // Line defines a face
            parseFace(line);                                                             // Analyse face data using helper function

            // Assign current material to this face
            if (currentMaterial.empty()) {
                printf("No material specified for this face\n");
                faceMaterials.push_back(-1);  // No material assigned
            }
            else if (materialMap.find(currentMaterial) == materialMap.end()) {
                printf("Material '%s' not found in material map\n", currentMaterial.c_str());
                faceMaterials.push_back(-1);  // Material not found
            }
            else {
                faceMaterials.push_back(materialMap[currentMaterial]);
            }           
        }
        else if (strncmp(line, "mtllib ", 7) == 0) {                                     // Line references a material library
            char mtlFilename[256];
            sscanf_s(line, "mtllib %s", mtlFilename, (unsigned int)sizeof(mtlFilename));

            // Combine directory with MTL filename
            mtlLibPath = directory + mtlFilename;

            printf("Loading material library: %s\n", mtlLibPath.c_str());

            // Load the material library
            loadMTL(mtlLibPath.c_str());
        }
        else if (strncmp(line, "usemtl ", 7) == 0) {                                     // Line specifies material to use
            char materialName[256];
            sscanf_s(line, "usemtl %s", materialName, (unsigned int)sizeof(materialName));
            currentMaterial = materialName;
            printf("Using material: %s\n", currentMaterial.c_str());
        }
    }

    fclose(file);

    printf("Loaded model: %s\n", filename);                                              // Print success message
    printf("Vertices: %zu, Texture Coords: %zu, Normals: %zu, Faces: %zu, Materials: %zu\n",
        vertices.size() - 1, textureCoords.size() - 1, normals.size() - 1, faces.size(), materials.size()); // Print model statistics
    return true;                                                                         // Return success
}

// Load MTL file and analyse its contents
bool ModelLoader::loadMTL(const char* filename) {
    FILE* file;
    errno_t err = fopen_s(&file, filename, "r");
    if (err != 0 || file == NULL) {
        printf("Error opening material file: %s\n", filename);
        return false;
    }

    printf("Successfully opened MTL file: %s\n", filename);

    // Extract directory path from the MTL filename
    std::string mtlPath = filename;
    size_t lastSlash = mtlPath.find_last_of("/\\");
    std::string directory = (lastSlash != std::string::npos) ? mtlPath.substr(0, lastSlash + 1) : "";

    char line[256];
    Material currentMaterial;
    bool inMaterial = false;

    while (fgets(line, sizeof(line), file)) {
        // Remove trailing newline
        size_t len = strlen(line);
        if (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
        }

        if (strncmp(line, "newmtl ", 7) == 0) {
            // If we were already analysing a material, add it to our list
            if (inMaterial) {
                materials.push_back(currentMaterial);
                materialMap[currentMaterial.name] = (int)materials.size() - 1;
                printf("Added material: %s (index: %d)\n", currentMaterial.name.c_str(), (int)materials.size() - 1);
            }

            // Start a new material
            currentMaterial = Material();
            
            // Fix: Properly extract material name as a string
            char materialName[256];
            sscanf_s(line, "newmtl %s", materialName, (unsigned int)sizeof(materialName));
            currentMaterial.name = materialName; // Assign to std::string properly
            
            printf("Parsing new material: %s\n", currentMaterial.name.c_str());
            inMaterial = true;
        }
        else if (inMaterial) {
            parseMTLLine(line, currentMaterial);

            // If this line defines a texture map, load the texture
            if (strncmp(line, "map_Kd ", 7) == 0) {
                char texturePath[256];
                sscanf_s(line, "map_Kd %s", texturePath, (unsigned int)sizeof(texturePath));

                // Combine directory with texture filename
                std::string fullTexturePath = directory + texturePath;
                currentMaterial.texturePath = fullTexturePath;

                printf("Attempting to load texture: %s\n", fullTexturePath.c_str());

                // Check if the texture file exists before trying to load it
                FILE* textureFile;
                errno_t textureErr = fopen_s(&textureFile, fullTexturePath.c_str(), "rb");
                if (textureErr == 0 && textureFile != NULL) {
                    fclose(textureFile);

                    // Load the texture
                    printf("TEST_TEST_TEST_TEST_TEST_TEST\n");
                    currentMaterial.textureID = loadTexture(fullTexturePath.c_str());
                    currentMaterial.hasTexture = (currentMaterial.textureID != 0);

                    if (currentMaterial.hasTexture) {
                        printf("Loaded texture: %s (ID: %u)\n", fullTexturePath.c_str(), currentMaterial.textureID);
                    }
                    else {
                        printf("Failed to load texture: %s\n", fullTexturePath.c_str());
                    }
                }
                else {
                    printf("Texture file not found: %s\n", fullTexturePath.c_str());
                    currentMaterial.hasTexture = false;
                }
            }
        }
    }

    // Add the last material if we were parsing one
    if (inMaterial) {
        materials.push_back(currentMaterial);
        materialMap[currentMaterial.name] = (int)materials.size() - 1;
        printf("Added final material: %s (index: %d)\n", currentMaterial.name.c_str(), (int)materials.size() - 1);
    }

    fclose(file);
    printf("Finished loading MTL file. Total materials: %zu\n", materials.size());
    return true;
}

// Parse a line from an MTL file
void ModelLoader::parseMTLLine(const char* line, Material& currentMaterial) {
    if (strncmp(line, "Ka ", 3) == 0) {
        // Ambient color
        sscanf_s(line, "Ka %f %f %f", &currentMaterial.ambient[0], &currentMaterial.ambient[1], &currentMaterial.ambient[2]);
        printf("  Ambient: %.2f, %.2f, %.2f\n", currentMaterial.ambient[0], currentMaterial.ambient[1], currentMaterial.ambient[2]);
    }
    else if (strncmp(line, "Kd ", 3) == 0) {
        // Diffuse color
        sscanf_s(line, "Kd %f %f %f", &currentMaterial.diffuse[0], &currentMaterial.diffuse[1], &currentMaterial.diffuse[2]);
        printf("  Diffuse: %.2f, %.2f, %.2f\n", currentMaterial.diffuse[0], currentMaterial.diffuse[1], currentMaterial.diffuse[2]);
    }
    else if (strncmp(line, "Ks ", 3) == 0) {
        // Specular color
        sscanf_s(line, "Ks %f %f %f", &currentMaterial.specular[0], &currentMaterial.specular[1], &currentMaterial.specular[2]);
        printf("  Specular: %.2f, %.2f, %.2f\n", currentMaterial.specular[0], currentMaterial.specular[1], currentMaterial.specular[2]);
    }
    else if (strncmp(line, "Ns ", 3) == 0) {
        // Shininess
        sscanf_s(line, "Ns %f", &currentMaterial.shininess);
        // Convert from MTL shininess (0-1000) to OpenGL shininess (0-128)
        currentMaterial.shininess = std::min(currentMaterial.shininess / 1000.0f * 128.0f, 128.0f);
        printf("  Shininess: %.2f\n", currentMaterial.shininess);
    }
}


// Load a texture from file
GLuint ModelLoader::loadTexture(const char* filename) {
    printf("Loading texture from: %s\n", filename);
    printf("000000000000000000000000000000000000000000000000\n");
    // Create a new texture ID
    GLuint textureID;
    glGenTextures(1, &textureID);

    // Bind the texture
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load the image using stb_image
    int width, height, channels;
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 0);

    if (data) {
        GLenum format;
        if (channels == 1)
            format = GL_RED;
        else if (channels == 3)
            format = GL_RGB;
        else if (channels == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        gluBuild2DMipmaps(GL_TEXTURE_2D, format, width, height, format, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);
        printf("Successfully loaded texture: %s (%dx%d, %d channels)\n", filename, width, height, channels);
    } else {
        printf("Failed to load texture: %s\n", filename);
        // Fall back to checkerboard pattern only if stb_image failed
        createCheckerboardTexture(textureID);
    }

    return textureID;
}

// Create a checkerboard pattern as placeholder texture
void ModelLoader::createCheckerboardTexture(GLuint textureID) {
    const int size = 64;
    unsigned char checkerboard[size][size][3];

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            // Create a more visible checkerboard pattern with larger squares
            bool isEvenCell = ((i / 8) % 2 == 0) ^ ((j / 8) % 2 == 0);

            if (isEvenCell) {
                // Light square - bright red
                checkerboard[i][j][0] = 255;  // Bright red
                checkerboard[i][j][1] = 50;   // Low green
                checkerboard[i][j][2] = 50;   // Low blue
            }
            else {
                // Dark square - bright blue
                checkerboard[i][j][0] = 50;   // Low red
                checkerboard[i][j][1] = 50;   // Low green
                checkerboard[i][j][2] = 255;  // Bright blue
            }
        }
    }

    // Bind and load the checkerboard data
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    // Set texture parameters explicitly
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Load the texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, checkerboard);
    
    // Generate mipmaps
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, size, size, GL_RGB, GL_UNSIGNED_BYTE, checkerboard);

    printf("Created checkerboard placeholder texture with bright red and blue colors\n");
}

// Helper function to parse face data from a line
void ModelLoader::parseFace(const char* line) {
    Face face;                                                                           // Create new face
    // Initialize all indices to 0
    for (int i = 0; i < 4; i++) {                                                        // Initialize all indices to 0
        face.vertexIndices[i] = face.textureIndices[i] = face.normalIndices[i] = 0;
    }

    // Try different face formats in order of complexity
    if (tryParseQuadWithTexAndNorm(line, face) ||                                        // Try quad with v/t/n 
        tryParseTriWithTexAndNorm(line, face) ||                                         // Try triangle with v/t/n 
        tryParseQuadWithNorm(line, face) ||                                              // Try quad with v/n 
        tryParseTriWithNorm(line, face) ||                                               // Try triangle with v/n 
        tryParseQuadVerticesOnly(line, face) ||                                          // Try quad with just vertices
        tryParseTriVerticesOnly(line, face)) {                                           // Try triangle with just vertices
        faces.push_back(face);                                                           // Add successfully parsed face
    }
}

// Try quad with texture and normal
bool ModelLoader::tryParseQuadWithTexAndNorm(const char* line, Face& face) {
    int matches = sscanf_s(line, "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",
        &face.vertexIndices[0], &face.textureIndices[0], &face.normalIndices[0],
        &face.vertexIndices[1], &face.textureIndices[1], &face.normalIndices[1],
        &face.vertexIndices[2], &face.textureIndices[2], &face.normalIndices[2],
        &face.vertexIndices[3], &face.textureIndices[3], &face.normalIndices[3]);

    if (matches == 12) {                                                                 // Successfully parsed as quad with v/t/n format
        face.vertexCount = 4;                                                            // Set as quad
        return true;
    }
    return false;
}

// Try triangle with texture and normal
bool ModelLoader::tryParseTriWithTexAndNorm(const char* line, Face& face) {
    int matches = sscanf_s(line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
        &face.vertexIndices[0], &face.textureIndices[0], &face.normalIndices[0],
        &face.vertexIndices[1], &face.textureIndices[1], &face.normalIndices[1],
        &face.vertexIndices[2], &face.textureIndices[2], &face.normalIndices[2]);

    if (matches == 9) {                                                                  // Successfully parsed as triangle with v/t/n format
        face.vertexCount = 3;                                                            // Set as triangle
        return true;
    }
    return false;
}

// Try quad with normal but no texture
bool ModelLoader::tryParseQuadWithNorm(const char* line, Face& face) {
    int matches = sscanf_s(line, "f %d//%d %d//%d %d//%d %d//%d",
        &face.vertexIndices[0], &face.normalIndices[0],
        &face.vertexIndices[1], &face.normalIndices[1],
        &face.vertexIndices[2], &face.normalIndices[2],
        &face.vertexIndices[3], &face.normalIndices[3]);

    if (matches == 8) {                                                                  // Successfully parsed as quad with v/n format
        face.vertexCount = 4;                                                            // Set as quad
        return true;
    }
    return false;
}

// Try triangle with normal but no texture
bool ModelLoader::tryParseTriWithNorm(const char* line, Face& face) {
    int matches = sscanf_s(line, "f %d//%d %d//%d %d//%d",
        &face.vertexIndices[0], &face.normalIndices[0],
        &face.vertexIndices[1], &face.normalIndices[1],
        &face.vertexIndices[2], &face.normalIndices[2]);

    if (matches == 6) {                                                                  // Successfully parsed as triangle with v/n format
        face.vertexCount = 3;                                                            // Set as triangle
        return true;
    }
    return false;
}

// Try quad with vertices only
bool ModelLoader::tryParseQuadVerticesOnly(const char* line, Face& face) {
    int matches = sscanf_s(line, "f %d %d %d %d",
        &face.vertexIndices[0], &face.vertexIndices[1],
        &face.vertexIndices[2], &face.vertexIndices[3]);

    if (matches == 4) {                                                                  // Successfully parsed as quad with just vertex indices
        face.vertexCount = 4;                                                            // Set as quad
        return true;
    }
    return false;
}

// Try triangle with vertices only
bool ModelLoader::tryParseTriVerticesOnly(const char* line, Face& face) {
    int matches = sscanf_s(line, "f %d %d %d",
        &face.vertexIndices[0], &face.vertexIndices[1],
        &face.vertexIndices[2]);

    if (matches == 3) {                                                                  // Successfully parsed as triangle with just vertex indices
        face.vertexCount = 3;                                                            // Set as triangle
        return true;
    }
    return false;
}

// Function to prompt user for a new OBJ file path and load it
void ModelLoader::loadNewModel() {
    char objFilePath[256] = "";                                                          // Buffer to store the file path input
    printf("Enter path to new OBJ file: ");                                              // Prompt user to enter file path
    if (fgets(objFilePath, sizeof(objFilePath), stdin) != NULL) {                        // Read input from standard input
        // Remove trailing newline if present
        size_t len = strlen(objFilePath);                                                // Get length of input string
        if (len > 0 && objFilePath[len - 1] == '\n') {                                   // Check if last character is newline
            objFilePath[len - 1] = '\0';                                                 // Replace newline with null terminator
        }

        if (strlen(objFilePath) > 0) {                                                   // Ensure path is not empty
            bool modelLoaded = loadOBJ(objFilePath);                                     // Attempt to load the model file
            if (modelLoaded) {                                                           // If model loaded successfully
                printf("Successfully loaded model: %s\n", objFilePath);                  // Display success message
            }
            else {                                                                       // If model failed to load
                printf("Failed to load OBJ file: %s\n", objFilePath);                    // Display error message
            }
        }
    }
}

// Modified drawModel function to support textures
void ModelLoader::drawModel() {
    // Apply model transformations
    glPushMatrix();
    glTranslatef(modelX, modelY, modelZ);
    glRotatef(modelRotX, 1.0f, 0.0f, 0.0f);
    glRotatef(modelRotY, 0.0f, 1.0f, 0.0f);
    glRotatef(modelRotZ, 0.0f, 0.0f, 1.0f);
    glScalef(modelScale, modelScale, modelScale);

    // Handle debug rendering modes
    if (debugMode == DEBUG_WIREFRAME) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_COLOR_MATERIAL);
        glColor3f(1.0f, 1.0f, 1.0f); // White wireframe
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    
    // Disable color material to ensure material properties take effect
    glDisable(GL_COLOR_MATERIAL);
    
    int lastMaterial = -1;  // Track the last material used to avoid redundant state changes
    
    // Draw each face
    for (size_t i = 0; i < faces.size(); i++) {
        // Apply material if available and different from the last one used
        int materialIndex = (i < faceMaterials.size()) ? faceMaterials[i] : -1;
        
        // For debug modes, override material settings
        if (debugMode == DEBUG_SOLID_COLOR) {
            // Use a solid color for all faces
            glDisable(GL_TEXTURE_2D);
            glEnable(GL_COLOR_MATERIAL);
            glColor3f(0.8f, 0.2f, 0.2f); // Bright red color
        } 
        else if (debugMode == DEBUG_CHECKERBOARD) {
            // Use checkerboard texture for all faces
            glEnable(GL_TEXTURE_2D);
            
            // Create checkerboard texture if it doesn't exist
            if (debugCheckerboardTextureID == 0) {
                glGenTextures(1, &debugCheckerboardTextureID);
                createCheckerboardTexture(debugCheckerboardTextureID);
            }
            
            glBindTexture(GL_TEXTURE_2D, debugCheckerboardTextureID);
            glEnable(GL_COLOR_MATERIAL);
            glColor3f(1.0f, 1.0f, 1.0f); // White color to show texture properly
        }
        else if (materialIndex >= 0 && materialIndex < materials.size() && materialIndex != lastMaterial && debugMode == DEBUG_NONE) {
            // Apply the material properties (only in normal rendering mode)
            Material& mat = materials[materialIndex];
            
            // Apply material properties
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat.ambient);
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat.diffuse);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat.specular);
            glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat.shininess);
            
            // Enable/bind texture if available
            if (mat.hasTexture && mat.textureID != 0) {
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, mat.textureID);
            } else {
                glDisable(GL_TEXTURE_2D);
            }
            
            lastMaterial = materialIndex;
        }
        
        // Draw the face
        const Face& face = faces[i];
        
        if (face.vertexCount == 3) {
            glBegin(GL_TRIANGLES);
        } else {
            glBegin(GL_QUADS);
        }
        
        for (int j = 0; j < face.vertexCount; j++) {
            // Set normal if available
            if (face.normalIndices[j] > 0 && face.normalIndices[j] < normals.size()) {
                const Normal& normal = normals[face.normalIndices[j]];
                glNormal3f(normal.x, normal.y, normal.z);
            }
            
            // Set texture coordinate if available and texturing is enabled
            if (face.textureIndices[j] > 0 && face.textureIndices[j] < textureCoords.size()) {
                const TextureCoord& texCoord = textureCoords[face.textureIndices[j]];
                
                // For texture coordinate debug mode, use the texture coordinates as colors
                if (debugMode == DEBUG_TEXCOORDS) {
                    glColor3f(texCoord.u, texCoord.v, 0.0f);
                } else {
                    glTexCoord2f(texCoord.u, texCoord.v);
                }
            }
            
            // Set vertex
            if (face.vertexIndices[j] > 0 && face.vertexIndices[j] < vertices.size()) {
                const Vertex& vertex = vertices[face.vertexIndices[j]];
                glVertex3f(vertex.x, vertex.y, vertex.z);
            }
        }
        
        glEnd();
        
        // Draw normals if in normal debug mode
        if (debugMode == DEBUG_NORMALS) {
            glDisable(GL_LIGHTING);
            glDisable(GL_TEXTURE_2D);
            glColor3f(0.0f, 1.0f, 0.0f); // Green for normals
            
            glBegin(GL_LINES);
            for (int j = 0; j < face.vertexCount; j++) {
                if (face.normalIndices[j] > 0 && face.normalIndices[j] < normals.size() &&
                    face.vertexIndices[j] > 0 && face.vertexIndices[j] < vertices.size()) {
                    
                    const Vertex& vertex = vertices[face.vertexIndices[j]];
                    const Normal& normal = normals[face.normalIndices[j]];
                    
                    // Draw a line representing the normal
                    glVertex3f(vertex.x, vertex.y, vertex.z);
                    glVertex3f(vertex.x + normal.x * 0.2f, 
                               vertex.y + normal.y * 0.2f, 
                               vertex.z + normal.z * 0.2f);
                }
            }
            glEnd();
            
            glEnable(GL_LIGHTING);
        }
    }
    
    // Restore state
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Reset polygon mode
    glEnable(GL_TEXTURE_2D);  // Restore texture state
    glEnable(GL_COLOR_MATERIAL);
    glPopMatrix();
}

// Function to draw a reference grid on the XZ plane
void ModelLoader::drawWireGrid(float size, int divisions, float y) {
    glDisable(GL_LIGHTING);                                                              // Turn off lighting for grid drawing

    // Set grid color based on debug mode
    if (debugMode == DEBUG_WIREFRAME) {
        glColor3f(0.0f, 0.7f, 0.7f); // Cyan color for grid in wireframe mode
    } else {
        glColor3f(0.7f, 0.7f, 0.7f); // Default gray color
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);                                           // Set polygon mode to wireframe

    float halfSize = size / 2.0f;                                                        // Calculate half of grid size
    float step = size / divisions;                                                       // Calculate cell size

    glBegin(GL_QUADS);                                                                   // Begin drawing quads
    for (int x = 0; x < divisions; x++) {                                                // Iterate through grid cells in X direction
        for (int z = 0; z < divisions; z++) {                                            // Iterate through grid cells in Z direction
            float x1 = -halfSize + x * step;                                             // Calculate left edge of cell
            float x2 = x1 + step;                                                        // Calculate right edge of cell
            float z1 = -halfSize + z * step;                                             // Calculate front edge of cell
            float z2 = z1 + step;                                                        // Calculate back edge of cell

            // Draw grid cell as a quad
            glVertex3f(x1, y, z1);                                                       // Bottom-left corner
            glVertex3f(x2, y, z1);                                                       // Bottom-right corner
            glVertex3f(x2, y, z2);                                                       // Top-right corner
            glVertex3f(x1, y, z2);                                                       // Top-left corner
        }
    }
    glEnd();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);                                           // Restore default polygon mode

    glEnable(GL_LIGHTING);                                                               // Turn lighting back on

    glColor3f(1.0f, 1.0f, 1.0f);
}

// Cycle between normal render, and 2 debug modes
void ModelLoader::cycleDebugMode() { 
    debugMode = (DebugRenderMode)((debugMode + 1) % 6); // Changed from 3 to 6 modes
    printf("Debug mode changed to: %d\n", debugMode);
    
    switch(debugMode) {
        case DEBUG_NONE:
            printf("Debug mode: NORMAL RENDERING\n");
            break;
        case DEBUG_SOLID_COLOR:
            printf("Debug mode: SOLID COLOR\n");
            break;
        case DEBUG_CHECKERBOARD:
            printf("Debug mode: CHECKERBOARD TEXTURE\n");
            break;
        case DEBUG_WIREFRAME:
            printf("Debug mode: WIREFRAME\n");
            break;
        case DEBUG_NORMALS:
            printf("Debug mode: NORMALS\n");
            break;
        case DEBUG_TEXCOORDS:
            printf("Debug mode: TEXTURE COORDINATES\n");
            break;
    }
}