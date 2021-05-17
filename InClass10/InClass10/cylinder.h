#pragma once

// Cylinder
//
// Drawing by primitive GL_TRIANGLES
//
//   min N = 2,  max N = 6;
//   # of subdivision (of 2pi) = pow(2,N)
//   # of triangles = (# of subdivision) * 2
//   # of vertices = (# of triangles) * 3
//   # of vertex coordinate values = (# of vertices) * 3
//
// Vertex shader: the location (0: position attrib (vec3), 1: color (vec3))
// Fragment shader: should catch the vertex color from the vertex shader

#ifndef CYLINDER_H
#define CYLINDER_H

#include <cmath>
#include <iostream>
#include "shader.h"

using namespace std;


class Cylinder {
    
public:
    const int MIN_N = 2;
    const int MAX_N = 6;
    const double PI = 3.141592;
    int N;
    int numSubdiv;       // numSubdiv = pow(2,N)
    int numVertices;     // numVertices = numSubdiv * 2 * 3
    float radius;
    float height;
    
    Cylinder() {
        N = MIN_N;
        numSubdiv = (int)pow(2.0, N);
        numVertices = numSubdiv * 6;
        radius = 1.0f;
        height = 1.0f;
        colorIndex = 0;
        createBuffers();
        updateBuffers();
    }
    
    Cylinder(int N, float radius, float height) {
        if (N < MIN_N || MAX_N < N) {
            cout << "Cylinder constructor error illegal N: " << N << endl;
            cout << "N must be in [" << MIN_N << ", " << MAX_N << "]" << endl;
            exit(-1);
        }
        this->N = N;
        this->numSubdiv = (int)pow(2.0, N);
        this->numVertices = this->numSubdiv * 6;
        this->radius = radius;
        this->height = height;
        colorIndex = 0;
        createBuffers();
        updateBuffers();
    }
    
    void draw(Shader *shader) {
        shader->use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, numVertices);
        glBindVertexArray(0);
    };

private:
    
    GLfloat vertices[1152];   // 64 * 2 * 3 * 3
    GLfloat colors[1152];     // 64 * 2 * 3 * 3
    GLfloat texcoords[384];   // 32 * 2 * 3 * 2, for InClass10
    GLfloat normal[1152];

    int colorIndex;

    float mainColors[15] = {
        .7f, .0f, .0f,
        .7f, .7f, .0f,
        .0f, .7f, .7f,
        .0f, .0f, .7f,
        .7f, .0f, .7f,
    };
    
    unsigned int VAO;
    // VBO[0]: for position, VBO[1]: for color, VBO[2]: texcoords (InClass06)
    unsigned int VBO[4];
    
    void createBuffers() {
        
        glGenVertexArrays(1, &VAO);
        glGenBuffers(4, VBO);        // 2 -> 3 for InClass08 (texture) 
        
        glBindVertexArray(VAO);
        
        // reserve space for position attributes
        glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), 0, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        // reserve space for normal coordinates: for InClass10
        glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(normal), 0, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // reserve space for color attributes
        glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(colors), 0, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        // reserve space for texture coordinates: for InClass10
        glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), 0, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        

        glBindVertexArray(0);
        
    }
    
    void updateBuffers() {
        
        // compute vertex attributes (for position and color)
        double angleStep = (PI * 2.0) / numSubdiv;
        double theta = 0.0;
        float halfHeight = height / 2.0;
        colorIndex = 0;
        
        // for texture coordinates (InClass06)
        float texStep = 1.0f / numSubdiv;
        float curTex = 0.0f;
        
        for (int i = 0; i < numSubdiv; i++) {
            int j = i * 18;   // j = i * 2 * 3 * 3 (for position, color)
            int k = colorIndex * 3;
            double phi = (i == (numSubdiv - 1)) ? 0.0 : (theta + angleStep);

            // for texture coordinates
            int q = i * 12;  
            float nextTex = (i == (numSubdiv - 1)) ? 1.0f : (curTex + texStep);
            
            // first triangle

            // first vertex
            vertices[j] = radius * cos(theta);
            vertices[j+1] = halfHeight;
            vertices[j+2] = radius * sin(theta);

            normal[j] = radius * cos(theta);
            normal[j + 1] = halfHeight;
            normal[j + 2] = radius * sin(theta);
            
            colors[j] = mainColors[k];
            colors[j+1] = mainColors[k+1];
            colors[j+2] = mainColors[k+2];
            
            // second vertex
            vertices[j+3] = vertices[j];
            vertices[j+4] = -halfHeight;
            vertices[j+5] = vertices[j+2];

            normal[j + 3] = vertices[j];
            normal[j + 4] = -halfHeight;
            normal[j + 5] = vertices[j + 2];
            
            colors[j+3] = mainColors[k];
            colors[j+4] = mainColors[k+1];
            colors[j+5] = mainColors[k+2];

            // third vertex
            vertices[j+6] = radius * cos(phi);
            vertices[j+7] = halfHeight;
            vertices[j+8] = radius * sin(phi);

            normal[j + 6] = radius * cos(phi);
            normal[j + 7] = halfHeight;
            normal[j + 8] = radius * sin(phi);
            
            colors[j+6] = mainColors[k];
            colors[j+7] = mainColors[k+1];
            colors[j+8] = mainColors[k+2];
            
            // texture coordinates (for first triangle)
            texcoords[q] = curTex;
            texcoords[q+1] = 1.0f;
            texcoords[q+2] = curTex;
            texcoords[q+3] = 0.0f;
            texcoords[q+4] = nextTex;
            texcoords[q+5] = 1.0f;
            
            // second triangle
            
            vertices[j+9] = vertices[j+6];
            vertices[j+10] = vertices[j+7];
            vertices[j+11] = vertices[j+8];

            normal[j + 9] = vertices[j + 6];
            normal[j + 10] = vertices[j + 7];
            normal[j + 11] = vertices[j + 8];
            
            colors[j+9] = mainColors[k];
            colors[j+10] = mainColors[k+1];
            colors[j+11] = mainColors[k+2];
            
            vertices[j+12] = vertices[j+3];
            vertices[j+13] = vertices[j+4];
            vertices[j+14] = vertices[j+5];

            normal[j + 12] = vertices[j + 3];
            normal[j + 13] = vertices[j + 4];
            normal[j + 14] = vertices[j + 5];
            
            colors[j+12] = mainColors[k];
            colors[j+13] = mainColors[k+1];
            colors[j+14] = mainColors[k+2];
            
            vertices[j+15] = vertices[j+6];
            vertices[j+16] = -halfHeight;
            vertices[j+17] = vertices[j+8];

            normal[j + 15] = vertices[j + 6];
            normal[j + 16] = -halfHeight;
            normal[j + 17] = vertices[j + 8];
            
            colors[j+15] = mainColors[k];
            colors[j+16] = mainColors[k+1];
            colors[j+17] = mainColors[k+2];

            // texture coordinates (for second triangle)
            texcoords[q+6] = nextTex;
            texcoords[q+7] = 1.0f;
            texcoords[q+8] = curTex;
            texcoords[q+9] = 0.0f;
            texcoords[q+10] = nextTex;
            texcoords[q+11] = 0.0f;
            
            // proceed to next step
            colorIndex = (colorIndex + 1) % 6;
            theta = theta + angleStep;
            curTex = curTex + texStep;
        }
        
        glBindVertexArray(VAO);
        
        glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(normal), normal);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(colors), colors);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(texcoords), texcoords);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
        glEnableVertexAttribArray(3);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        glBindVertexArray(0);
    };
    
};


#endif
