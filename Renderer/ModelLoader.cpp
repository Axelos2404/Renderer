#include "ModelLoader.h"
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <freeglut.h>

// Model data containers
std::vector<Vertex> vertices = { {0, 0, 0} };                                            // Start with dummy vertex at index 0
std::vector<TextureCoord> textureCoords = { {0, 0} };                                    // Start with dummy texture coordinate at index 0
std::vector<Normal> normals = { {0, 0, 0} };                                             // Start with dummy normal at index 0
std::vector<Face> faces;                                                                 // Collection of faces

// Model transformation variables
float modelX = 0.0f, modelY = 0.0f, modelZ = 0.0f;                                       // Model position
float modelRotX = 0.0f, modelRotY = 0.0f, modelRotZ = 0.0f;                              // Model rotation angles
float modelScale = 1.0f;                                                                 // Model scale factor

// Reset model transformations to default values
void resetModel() {
    modelX = 0.0f;                                                                       // Reset X position
    modelY = 0.0f;                                                                       // Reset Y position
    modelZ = 0.0f;                                                                       // Reset Z position
    modelRotX = 0.0f;                                                                    // Reset X rotation
    modelRotY = 0.0f;                                                                    // Reset Y rotation
    modelRotZ = 0.0f;                                                                    // Reset Z rotation
    modelScale = 1.0f;                                                                   // Reset scale
}

// Load OBJ file and parse its contents
bool loadOBJ(const char* filename) {
    FILE* file;                                                                          // File handle
    errno_t err = fopen_s(&file, filename, "r");                                         // Open file for reading
    if (err != 0 || file == NULL) {                                                      // Check if file opened successfully
        printf("Error opening file: %s\n", filename);                                    // Print error message
        return false;                                                                    // Return failure
    }

    // Clear previous model data
    vertices = { {0, 0, 0} };                                                            // Reset vertices with dummy at index 0
    textureCoords = { {0, 0} };                                                          // Reset texture coordinates with dummy at index 0
    normals = { {0, 0, 0} };                                                             // Reset normals with dummy at index 0
    faces.clear();                                                                       // Clear all faces

    // Reset model transformations
    resetModel();                                                                        // Reset position, rotation, and scale

    char line[256];                                                                      // Buffer for reading lines from file
    while (fgets(line, sizeof(line), file)) {                                            // Read file line by line
        // Process vertex data
        if (strncmp(line, "v ", 2) == 0) {                                               // Line defines a vertex
            Vertex vertex;                                                               // Create new vertex
            sscanf_s(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);              // Parse vertex coordinates
            vertices.push_back(vertex);                                                  // Add vertex to collection
        }
        else if (strncmp(line, "vt ", 3) == 0) {                                         // Line defines a texture coordinate
            TextureCoord texCoord;                                                       // Create new texture coordinate
            sscanf_s(line, "vt %f %f", &texCoord.u, &texCoord.v);                        // Parse texture coordinates
            textureCoords.push_back(texCoord);                                           // Add texture coordinate to collection
        }
        else if (strncmp(line, "vn ", 3) == 0) {                                         // Line defines a normal vector
            Normal normal;                                                               // Create new normal
            sscanf_s(line, "vn %f %f %f", &normal.x, &normal.y, &normal.z);             // Parse normal vector
            normals.push_back(normal);                                                   // Add normal to collection
        }
        else if (strncmp(line, "f ", 2) == 0) {                                          // Line defines a face
            Face face;                                                                   // Create new face
            // Initialize all indices to 0
            for (int i = 0; i < 4; i++) {                                                // Initialize all indices to 0
                face.vertexIndices[i] = face.textureIndices[i] = face.normalIndices[i] = 0;
            }

            // Try to parse as a quad first (4 vertices with v/t/n format)
            int matches = sscanf_s(line, "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",
                &face.vertexIndices[0], &face.textureIndices[0], &face.normalIndices[0],
                &face.vertexIndices[1], &face.textureIndices[1], &face.normalIndices[1],
                &face.vertexIndices[2], &face.textureIndices[2], &face.normalIndices[2],
                &face.vertexIndices[3], &face.textureIndices[3], &face.normalIndices[3]);

            if (matches == 12) {                                                         // Successfully parsed as quad with v/t/n format
                face.vertexCount = 4;                                                    // Set as quad (4 vertices)
                faces.push_back(face);                                                   // Add face to collection
            }
            else {
                // Try as a triangle with v/t/n format
                matches = sscanf_s(line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
                    &face.vertexIndices[0], &face.textureIndices[0], &face.normalIndices[0],
                    &face.vertexIndices[1], &face.textureIndices[1], &face.normalIndices[1],
                    &face.vertexIndices[2], &face.textureIndices[2], &face.normalIndices[2]);

                if (matches == 9) {                                                      // Successfully parsed as triangle with v/t/n format
                    face.vertexCount = 3;                                                // Set as triangle (3 vertices)
                    faces.push_back(face);                                               // Add face to collection
                }
                else {
                    // Try v//n format (quad) - vertices and normals but no texture coordinates
                    matches = sscanf_s(line, "f %d//%d %d//%d %d//%d %d//%d",
                        &face.vertexIndices[0], &face.normalIndices[0],
                        &face.vertexIndices[1], &face.normalIndices[1],
                        &face.vertexIndices[2], &face.normalIndices[2],
                        &face.vertexIndices[3], &face.normalIndices[3]);

                    if (matches == 8) {                                                  // Successfully parsed as quad with v//n format
                        face.vertexCount = 4;                                            // Set as quad (4 vertices)
                        faces.push_back(face);                                           // Add face to collection
                    }
                    else {
                        // Try v//n format (triangle) - vertices and normals but no texture coordinates
                        matches = sscanf_s(line, "f %d//%d %d//%d %d//%d",
                            &face.vertexIndices[0], &face.normalIndices[0],
                            &face.vertexIndices[1], &face.normalIndices[1],
                            &face.vertexIndices[2], &face.normalIndices[2]);

                        if (matches == 6) {                                              // Successfully parsed as triangle with v//n format
                            face.vertexCount = 3;                                        // Set as triangle (3 vertices)
                            faces.push_back(face);                                       // Add face to collection
                        }
                        else {
                            // Try simple format (quad) - just vertex indices
                            matches = sscanf_s(line, "f %d %d %d %d",
                                &face.vertexIndices[0], &face.vertexIndices[1],
                                &face.vertexIndices[2], &face.vertexIndices[3]);

                            if (matches == 4) {                                          // Successfully parsed as quad with just vertex indices
                                face.vertexCount = 4;                                    // Set as quad (4 vertices)
                                faces.push_back(face);                                   // Add face to collection
                            }
                            else {
                                // Try simple format (triangle) - just vertex indices
                                matches = sscanf_s(line, "f %d %d %d",
                                    &face.vertexIndices[0], &face.vertexIndices[1],
                                    &face.vertexIndices[2]);

                                if (matches == 3) {                                      // Successfully parsed as triangle with just vertex indices
                                    face.vertexCount = 3;                                // Set as triangle (3 vertices)
                                    faces.push_back(face);                               // Add face to collection
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    fclose(file);                                                                        // Close the file
    printf("Loaded model: %s\n", filename);                                              // Print success message
    printf("Vertices: %zu, Texture Coords: %zu, Normals: %zu, Faces: %zu\n",
        vertices.size() - 1, textureCoords.size() - 1, normals.size() - 1, faces.size()); // Print model statistics
    return true;                                                                         // Return success
}

#if _WIN64
// Load an FBX format 3D model file
bool loadFBX(const char* filename) {
    // Clear existing model data
    vertices.clear();
    textureCoords.clear();
    normals.clear();
    faces.clear();

    // Add dummy elements at index 0 since FBX indices start at 0 but our system expects 1-based
    vertices.push_back({ 0.0f, 0.0f, 0.0f });
    textureCoords.push_back({ 0.0f, 0.0f });
    normals.push_back({ 0.0f, 0.0f, 0.0f });

    // Initialize FBX SDK manager
    FbxManager* fbxManager = FbxManager::Create();
    if (!fbxManager) {
        printf("Error: Unable to create FBX Manager!\n");
        return false;
    }

    // Create an IOSettings object
    FbxIOSettings* ios = FbxIOSettings::Create(fbxManager, IOSROOT);
    fbxManager->SetIOSettings(ios);

    // Create an importer
    FbxImporter* importer = FbxImporter::Create(fbxManager, "");

    // Initialize the importer with the file
    bool importStatus = importer->Initialize(filename, -1, fbxManager->GetIOSettings());
    if (!importStatus) {
        printf("Error: Unable to initialize FBX importer for %s\n", filename);
        printf("Error: %s\n", importer->GetStatus().GetErrorString());
        importer->Destroy();
        fbxManager->Destroy();
        return false;
    }

    // Create a new scene
    FbxScene* scene = FbxScene::Create(fbxManager, "myScene");

    // Import the contents of the file into the scene
    importer->Import(scene);
    importer->Destroy();

    // Get the root node of the scene
    FbxNode* rootNode = scene->GetRootNode();
    if (rootNode) {
        // Process all nodes in the scene recursively
        for (int i = 0; i < rootNode->GetChildCount(); i++) {
            ProcessFbxNode(rootNode->GetChild(i));
        }
    }

    // Clean up
    scene->Destroy();
    fbxManager->Destroy();

    printf("FBX model loaded: %s\n", filename);
    printf("Vertices: %zu, Texture Coords: %zu, Normals: %zu, Faces: %zu\n",
        vertices.size() - 1, textureCoords.size() - 1, normals.size() - 1, faces.size());

    return true;
}
#endif

#if _WIN64
// Process a node in the FBX scene
void ProcessFbxNode(FbxNode* node) {
    // Get the node's global transform
    FbxAMatrix globalTransform = node->EvaluateGlobalTransform();

    // Get the node's mesh if it exists
    FbxMesh* mesh = node->GetMesh();
    if (mesh) {
        // Process mesh geometry
        int vertexCount = mesh->GetControlPointsCount();
        FbxVector4* controlPoints = mesh->GetControlPoints();

        // Store the starting index for this mesh's vertices
        int vertexOffset = vertices.size();

        // Add all vertices from this mesh with proper transformation
        for (int i = 0; i < vertexCount; i++) {
            // Apply global transformation to the vertex
            FbxVector4 transformedPoint = globalTransform.MultT(controlPoints[i]);

            Vertex v;
            v.x = static_cast<float>(transformedPoint[0]);
            v.y = static_cast<float>(transformedPoint[1]);
            v.z = static_cast<float>(transformedPoint[2]);
            vertices.push_back(v);
        }

        // Get all polygon (face) information
        int polygonCount = mesh->GetPolygonCount();
        for (int i = 0; i < polygonCount; i++) {
            // Get number of vertices in this polygon
            int polygonSize = mesh->GetPolygonSize(i);
            if (polygonSize > 4) {
                // Skip polygons with more than 4 vertices (not supported in our renderer)
                printf("Warning: Skipping polygon with %d vertices (max supported is 4)\n", polygonSize);
                continue;
            }

            Face face;
            face.vertexCount = polygonSize;

            // Get each vertex of the polygon
            for (int j = 0; j < polygonSize; j++) {
                // Get control point (vertex) index
                int controlPointIndex = mesh->GetPolygonVertex(i, j);
                face.vertexIndices[j] = controlPointIndex + vertexOffset;

                // Get UV coordinates if available
                FbxLayerElementUV* uvElement = mesh->GetElementUV(0);
                if (uvElement) {
                    int uvIndex = 0;
                    switch (uvElement->GetMappingMode()) {
                    case FbxLayerElement::eByControlPoint:
                        uvIndex = controlPointIndex;
                        break;
                    case FbxLayerElement::eByPolygonVertex:
                        uvIndex = mesh->GetTextureUVIndex(i, j);
                        break;
                    default:
                        break;
                    }

                    FbxVector2 uv;
                    if (uvElement->GetReferenceMode() == FbxLayerElement::eDirect) {
                        uv = uvElement->GetDirectArray().GetAt(uvIndex);
                    }
                    else if (uvElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect) {
                        int id = uvElement->GetIndexArray().GetAt(uvIndex);
                        uv = uvElement->GetDirectArray().GetAt(id);
                    }

                    TextureCoord tc;
                    tc.u = static_cast<float>(uv[0]);
                    tc.v = static_cast<float>(uv[1]);
                    textureCoords.push_back(tc);
                    face.textureIndices[j] = textureCoords.size() - 1;
                }
                else {
                    face.textureIndices[j] = 0; // No texture coordinates
                }

                // Get normal if available
                FbxLayerElementNormal* normalElement = mesh->GetElementNormal(0);
                if (normalElement) {
                    FbxVector4 normal;
                    if (normalElement->GetMappingMode() == FbxLayerElement::eByControlPoint) {
                        switch (normalElement->GetReferenceMode()) {
                        case FbxLayerElement::eDirect:
                            normal = normalElement->GetDirectArray().GetAt(controlPointIndex);
                            break;
                        case FbxLayerElement::eIndexToDirect:
                            int id = normalElement->GetIndexArray().GetAt(controlPointIndex);
                            normal = normalElement->GetDirectArray().GetAt(id);
                            break;
                        }
                    }
                    else if (normalElement->GetMappingMode() == FbxLayerElement::eByPolygonVertex) {
                        switch (normalElement->GetReferenceMode()) {
                        case FbxLayerElement::eDirect:
                            normal = normalElement->GetDirectArray().GetAt(mesh->GetPolygonVertexIndex(i) + j);
                            break;
                        case FbxLayerElement::eIndexToDirect:
                            int id = normalElement->GetIndexArray().GetAt(mesh->GetPolygonVertexIndex(i) + j);
                            normal = normalElement->GetDirectArray().GetAt(id);
                            break;
                        }
                    }

                    Normal n;
                    n.x = static_cast<float>(normal[0]);
                    n.y = static_cast<float>(normal[1]);
                    n.z = static_cast<float>(normal[2]);
                    normals.push_back(n);
                    face.normalIndices[j] = normals.size() - 1;
                }
                else {
                    face.normalIndices[j] = 0; // No normal
                }
            }

            faces.push_back(face);
        }
    }

    // Process child nodes recursively
    for (int i = 0; i < node->GetChildCount(); i++) {
        ProcessFbxNode(node->GetChild(i));
    }
}
#endif

// Prompt user for a new model file path and load it
void loadNewModel() {
    char filename[256];
    printf("Enter model file path (OBJ or FBX format): ");
    scanf_s("%255s", filename, (unsigned)_countof(filename));

    // Get file extension
    const char* extension = strrchr(filename, '.');
    if (!extension) {
        printf("Error: File has no extension. Please specify .obj or .fbx file.\n");
        return;
    }

    // Load based on file extension
    bool success = false;
    if (_stricmp(extension, ".obj") == 0) {
        success = loadOBJ(filename);
    }
#if _WIN64
    else if (_stricmp(extension, ".fbx") == 0) {
        success = loadFBX(filename);
    }
#endif
    else {
        printf("Error: Unsupported file format. Only .obj and .fbx are supported.\n");
        return;
    }

    if (success) {
        // Reset model position and orientation after loading
        resetModel();
    }
    else {
        printf("Failed to load model: %s\n", filename);
    }
}

// Function to render the 3D model with current transformations
void drawModel() {
    // Enable two-sided rendering for better model visibility
    glDisable(GL_CULL_FACE);                                                             // Disable face culling to show both sides

    // Apply model transformations in proper order: scale, rotate, translate
    glPushMatrix();                                                                      // Save current transformation matrix
    glTranslatef(modelX, modelY, modelZ);                                                // Apply translation to position model
    glRotatef(modelRotX, 1.0f, 0.0f, 0.0f);                                              // Apply rotation around X axis
    glRotatef(modelRotY, 0.0f, 1.0f, 0.0f);                                              // Apply rotation around Y axis
    glRotatef(modelRotZ, 0.0f, 0.0f, 1.0f);                                              // Apply rotation around Z axis
    glScalef(modelScale, modelScale, modelScale);                                        // Apply uniform scaling

    // Iterate through all faces in the model
    for (const auto& face : faces) {
        if (face.vertexCount == 3) {                                                     // If face is a triangle
            // Draw triangle
            glBegin(GL_TRIANGLES);                                                       // Begin triangle primitive
            for (int i = 0; i < 3; i++) {                                                // Process each vertex of the triangle
                // Apply normal vector for lighting if available
                if (face.normalIndices[i] > 0 && face.normalIndices[i] < normals.size()) {
                    const Normal& normal = normals[face.normalIndices[i]];               // Get normal vector
                    glNormal3f(normal.x, normal.y, normal.z);                            // Set normal for lighting calculations
                }

                // Apply texture coordinate if available
                if (face.textureIndices[i] > 0 && face.textureIndices[i] < textureCoords.size()) {
                    const TextureCoord& texCoord = textureCoords[face.textureIndices[i]]; // Get texture coordinate
                    glTexCoord2f(texCoord.u, texCoord.v);                                // Set texture coordinate for this vertex
                }

                // Set vertex position
                if (face.vertexIndices[i] > 0 && face.vertexIndices[i] < vertices.size()) {
                    const Vertex& vertex = vertices[face.vertexIndices[i]];              // Get vertex
                    glVertex3f(vertex.x, vertex.y, vertex.z);                            // Set vertex position
                }
            }
            glEnd();                                                                     // End triangle primitive
        }
        else if (face.vertexCount == 4) {                                                // If face is a quad
            // Draw quad
            glBegin(GL_QUADS);                                                           // Begin quad primitive
            for (int i = 0; i < 4; i++) {                                                // Process each vertex of the quad
                // Apply normal vector for lighting if available
                if (face.normalIndices[i] > 0 && face.normalIndices[i] < normals.size()) {
                    const Normal& normal = normals[face.normalIndices[i]];               // Get normal vector
                    glNormal3f(normal.x, normal.y, normal.z);                            // Set normal for lighting calculations
                }

                // Apply texture coordinate if available
                if (face.textureIndices[i] > 0 && face.textureIndices[i] < textureCoords.size()) {
                    const TextureCoord& texCoord = textureCoords[face.textureIndices[i]]; // Get texture coordinate
                    glTexCoord2f(texCoord.u, texCoord.v);                                // Set texture coordinate for this vertex
                }

                // Set vertex position
                if (face.vertexIndices[i] > 0 && face.vertexIndices[i] < vertices.size()) {
                    const Vertex& vertex = vertices[face.vertexIndices[i]];              // Get vertex
                    glVertex3f(vertex.x, vertex.y, vertex.z);                            // Set vertex position
                }
            }
            glEnd();                                                                     // End quad primitive
        }
    }

    glPopMatrix();                                                                       // Restore previous transformation matrix
}

// Function to draw a reference grid on the XZ plane
void drawWireGrid(float size, int divisions, float y) {
    // Disable lighting for the grid to ensure consistent appearance
    glDisable(GL_LIGHTING);                                                              // Turn off lighting for grid drawing

    // Set grid color (light gray)
    glColor3f(0.7f, 0.7f, 0.7f);                                                         // Set color to light gray

    // Draw in wireframe mode
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
    glEnd();                                                                             // End quad drawing

    // Reset polygon mode to filled
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);                                           // Restore default polygon mode

    // Re-enable lighting for subsequent rendering
    glEnable(GL_LIGHTING);                                                               // Turn lighting back on

    // Reset color to white
    glColor3f(1.0f, 1.0f, 1.0f);                                                         // Reset color to white
}