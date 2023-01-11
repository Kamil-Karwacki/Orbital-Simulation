#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>
#include "mesh.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>
#include <iostream>
#include <algorithm>

#include "globals.hpp"
#include "math.hpp"

void SetupMesh(Mesh &mesh) {
    std::cout << "Setting up mesh" << std::endl;
    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffers(1, &mesh.VBO);
    glGenBuffers(1, &mesh.EBO);

    glBindVertexArray(mesh.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);

    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), &mesh.vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), &mesh.indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::normal));

    glEnableVertexAttribArray(2);	
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::color));
    
    glBindVertexArray(0);
}

void DrawMesh(Mesh mesh, unsigned int shaderProgram) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, mesh.position);
    model = glm::scale(model, mesh.scale);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);


    glBindVertexArray(mesh.VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

Mesh CreateIcoSphere(unsigned int depth) {
    Mesh mesh;

    glm::vec3 vertices[] = {
        glm::vec3( 0.0,  0.0,  1.0), // forward
        glm::vec3( 1.0,  0.0,  0.0), // right
        glm::vec3( 0.0,  0.0, -1.0), // back
        glm::vec3(-1.0,  0.0,  0.0), // left
        glm::vec3( 0.0,  1.0,  0.0), // up
        glm::vec3( 0.0, -1.0,  0.0)  // down
    };
    
    unsigned int indices[] = {4,0,2,4,2,1,4,1,3,4,3,0,5,2,0,5,1,2,5,3,1,5,0,3};

    
    // Generate base for octahedron
    for(int v=0; v<4; v++) {
        for(int i=0; i<depth+1; i++) {
            float ratio = (1.0f/(depth+1)) * i;
            Vertex newVertex{};
            if(v==3)
                newVertex.position = Interpolate(vertices[3], vertices[0], ratio);
            else
                newVertex.position = Interpolate(vertices[v], vertices[v+1], ratio);
            mesh.vertices.emplace_back(newVertex);
        }
    }

    int topTriangleVertices;
    int bottomTriangleVertices;

    // Create vertices for top part of octahedron
    for(int i=1; i<depth+1; i++) {
        float ratio = (1.0f/(depth+1)) * i;
        for(int l=0; l<4; l++) {  // for every one of four edges
            glm::vec3 newLayerPos = Interpolate(vertices[l], vertices[4], ratio); // current corner
            glm::vec3 newLayerPos2; // next corner
            if(l==3)
                newLayerPos2 = Interpolate(vertices[0], vertices[4], ratio);
            else 
                newLayerPos2 = Interpolate(vertices[l+1], vertices[4], ratio);

            for(int j=0; j<depth-i+1; j++) {
                float ratio2 = (1.0f/(depth+1-i)) * j;
                Vertex newVertex{};
                newVertex.position = Interpolate(newLayerPos, newLayerPos2, ratio2); // interpolate to the next edge
                mesh.vertices.emplace_back(newVertex);
            }
        }
        if(i==depth) {
            Vertex top{};
            top.position = vertices[4];
            mesh.vertices.emplace_back(top);
        }
    }
    topTriangleVertices = mesh.vertices.size();

    // Creating vertices for bottom part of octahedron
    for(int i=1; i<depth+1; i++) { 
        float ratio = (1.0f/(depth+1)) * i;
        for(int l=0; l<4; l++) { // for every one of four edges
            glm::vec3 newLayerPos = Interpolate(vertices[l], vertices[5], ratio); // current corner
            glm::vec3 newLayerPos2; // next corner
            if(l==3)
                newLayerPos2 = Interpolate(vertices[0], vertices[5], ratio);
            else 
                newLayerPos2 = Interpolate(vertices[l+1], vertices[5], ratio);
            for(int j=0; j<depth-i+1; j++) {
                float ratio2 = (1.0f/(depth+1-i)) * j;
                Vertex newVertex{};
                newVertex.position = Interpolate(newLayerPos, newLayerPos2, ratio2); // interpolate to the next edge
                mesh.vertices.emplace_back(newVertex);
            }
        }
        if(i==depth) {
            Vertex bottom{};
            bottom.position = vertices[5];
            mesh.vertices.emplace_back(bottom);
        }
    }
    bottomTriangleVertices = mesh.vertices.size() - topTriangleVertices;

    //// Im gonna refactor it ... sometime //// 
    //// Dont touch it or it will break ////
    int layer = 0; // Current row of vertices
    int currentLayer = 0; // Current layer's first index
    int nextLayer = 4 + 4 * (depth-layer); // Next layer first index
    int previousLayerIndex = nextLayer; // Previouse layer first index
    int sideLength = 0, originalSideLength; // Number of vertices in current side

    int lastIndex = 4 + 4 * (depth - layer); // Last index in 
    int cornerCount = 0; // Number of corners program have looped over. Set to 0 at every new layer
    for(int i=0; i<topTriangleVertices-1; i++) {
        if(i == currentLayer) { // if its new layer
            sideLength = i + depth + 1 - layer;
            originalSideLength = depth + 1 - layer;
        }

        if(i == sideLength) {
            sideLength += originalSideLength;
            cornerCount++;
        }

        if(i == lastIndex-1) { // if its last vertex in current layer
            mesh.indices.emplace_back(i);
            mesh.indices.emplace_back(currentLayer);   
            mesh.indices.emplace_back(i+1);
            if(layer != 0) {
                mesh.indices.emplace_back(i);
                mesh.indices.emplace_back(i-previousLayerIndex + cornerCount+1);
                mesh.indices.emplace_back(currentLayer);
            } 
            layer++;
            previousLayerIndex = nextLayer;
            currentLayer += nextLayer;
            nextLayer = 4 + 4 * (depth-layer);
            lastIndex += nextLayer;
            cornerCount = 0;
            sideLength = 0;
            originalSideLength = 0;
            
        } else {
            mesh.indices.emplace_back(i);
            mesh.indices.emplace_back(i+1);    
            mesh.indices.emplace_back(i + nextLayer - cornerCount);
            if(layer != 0) {
                mesh.indices.emplace_back(i);
                mesh.indices.emplace_back(i-previousLayerIndex + cornerCount+1);
                mesh.indices.emplace_back(i+1);
            }
        }
    }

    layer = 0; // current row of vertices
    currentLayer = 0; // current layer first index
    nextLayer = 4 + 4 * (depth-layer); // nex layer first index
    previousLayerIndex = nextLayer;
    sideLength = 0, originalSideLength=0;
    cornerCount = 0;

    for(int i=0; i<mesh.vertices.size(); i++) {
        if(i==mesh.vertices.size()-1) 
            break;
        
        if(i == currentLayer || i+topTriangleVertices == currentLayer) { // if its new layer
            sideLength = i + depth + 1 - layer;
            originalSideLength = depth + 1 - layer;
        }

        if(i == sideLength || i+topTriangleVertices == sideLength) { // if its corener
            sideLength += originalSideLength;
            cornerCount++;
        }
        if(i == nextLayer-1) { // if its last vertex in current layer
            if(layer == 0) {
                mesh.indices.emplace_back(i);
                mesh.indices.emplace_back(topTriangleVertices);
                mesh.indices.emplace_back(0);
                i = topTriangleVertices-1;
                layer++;
                previousLayerIndex = 0;
                currentLayer += topTriangleVertices;
                nextLayer = topTriangleVertices + 4 + 4 * (depth-layer);
                cornerCount = 0;
                sideLength = 0;
                originalSideLength = 0;
            } else {
                mesh.indices.emplace_back(i);
                mesh.indices.emplace_back(i+1);
                mesh.indices.emplace_back(currentLayer);

                if(layer != 0) {
                    mesh.indices.emplace_back(i);
                    mesh.indices.emplace_back(currentLayer);
                    mesh.indices.emplace_back(previousLayerIndex + (i-currentLayer) + cornerCount+1);
                }

                layer++;
                previousLayerIndex = currentLayer;
                currentLayer = i+1;
                nextLayer += 4 + 4 * (depth-layer);
                cornerCount = 0;
                sideLength = 0;
                originalSideLength = 0;
            }
            
        } else {
            if(layer==0) {
                mesh.indices.emplace_back(i);
                mesh.indices.emplace_back(topTriangleVertices + i - cornerCount);
                mesh.indices.emplace_back(i+1);
                continue;
            } else {
                mesh.indices.emplace_back(i);
                mesh.indices.emplace_back(i+1);
                mesh.indices.emplace_back(previousLayerIndex + (i-currentLayer) + cornerCount+1);
            }
                
            mesh.indices.emplace_back(i);
            mesh.indices.emplace_back(nextLayer + (i - currentLayer) - cornerCount);
            mesh.indices.emplace_back(i+1);    

        }
    }
    glm::vec3 center = glm::vec3(0.0f);

    for(int i=0; i<mesh.vertices.size(); i++) {
        glm::vec3 pointer = mesh.vertices[i].position - center;
        mesh.vertices[i].normal = pointer;
        float distance = glm::length(pointer);

        mesh.vertices[i].position = (1.0f/distance) * pointer;
    }

    std::cout << "Created ico sphere vertices: " << mesh.vertices.size() << " indices: " << mesh.indices.size() << " " << glfwGetTime() << "\n";
    
    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffers(1, &mesh.VBO);
    glGenBuffers(1, &mesh.EBO);

    glBindVertexArray(mesh.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);

    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), &mesh.vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), &mesh.indices[0], GL_STATIC_DRAW);


    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::normal));

    glEnableVertexAttribArray(2);	
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::color));

    glBindVertexArray(0);
    return mesh;
}