#pragma once


#ifndef CONE_H
#define CONE_H

#define _USE_MATH_DEFINES
#define NUMOFTRIANGLE 32

#include <cmath>
#include <iostream>
#include "shader.h"

using namespace std;

float calAngle(int i) {
	return (2 * M_PI * i) / NUMOFTRIANGLE;
}

class Cone {

public:
	const double PI = M_PI;
	int numVertices;
	float radius;
	bool smoothShading;

	Cone() {
		radius = 1.0f;
		smoothShading = false;
		createBuffers();
		updateBuffers();
	}

	void draw(Shader* shader) {
		shader->use();
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 32*3);
		glBindVertexArray(0);
	};

	void updateBuffers(bool smoothShading) {
		this->smoothShading = smoothShading;
		cout << "shading type update" << endl;
		updateBuffers();
	}

private:
	GLfloat vertices[396];  // 33 * 1 * 4 * 3

	GLfloat normalVectors[396];

	GLfloat colors[396];     // 33 * 1 * 4 * 3

	float mainColors[4] = 
	{
		1.0f, 0.5f, 0.31f, 1.0f
	};

	unsigned int VAO;
	unsigned int VBO[3];      // VBO[0]: for position, VBO[1]: for normal, VBO[2]: for color

	void createBuffers() {

		glGenVertexArrays(1, &VAO);
		glGenBuffers(3, VBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), 0, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(normalVectors), 0, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(colors), 0, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);

	}

	void updateBuffers() {
		GLfloat temp[3] = {0.0f, 0.0f, 0.0f};
		for (int i = 0; i <= NUMOFTRIANGLE; i++) {
			temp[0] += 2.0f * (sin(calAngle(i + 1)) - sin(calAngle(i)));
			temp[1] += -1.0f * (cos(calAngle(i + 1)) * sin(calAngle(i)) - cos(calAngle(i)) * sin(calAngle(i + 1)));
			temp[2] += 2.0f * (-1.0f * cos(calAngle(i + 1)) + cos(calAngle(i)));
		}

		for (int i = 0; i < 3; i++) {
			temp[i] = temp[i] / (NUMOFTRIANGLE + 1);
		}
		
		for (int i = 0; i < NUMOFTRIANGLE; i++) {
			vertices[12 * i + 0] = 0.0f;
			vertices[12 * i + 1] = 1.0f;
			vertices[12 * i + 2] = 0.0f;
			vertices[12 * i + 3] = 1.0f;

			vertices[12 * i + 4] = radius * cos(calAngle(i));
			vertices[12 * i + 5] = -1.0f;
			vertices[12 * i + 6] = radius * sin(calAngle(i));
			vertices[12 * i + 7] = 1.0f;

			vertices[12 * i + 8] = radius * cos(calAngle(i + 1));
			vertices[12 * i + 9] = -1.0f;
			vertices[12 * i + 10] = radius * sin(calAngle(i + 1));
			vertices[12 * i + 11] = 1.0f;
			

			for (int p = 0; p < 4; p++) {
				colors[12 * i + 4 * p + 0] = mainColors[0];
				colors[12 * i + 4 * p + 1] = mainColors[1];
				colors[12 * i + 4 * p + 2] = mainColors[2];
				colors[12 * i + 4 * p + 3] = mainColors[3];
			}
			
			GLfloat x = 2.0f * (sin(calAngle(i + 1)) - sin(calAngle(i)));
			GLfloat y = -1.0f * (cos(calAngle(i + 1)) * sin(calAngle(i)) - cos(calAngle(i)) * sin(calAngle(i + 1)));
			GLfloat z = 2.0f * (-1.0f*cos(calAngle(i + 1)) + cos(calAngle(i)));

			if (!smoothShading) { // normalVectors for flat shading
				normalVectors[12 * i + 0] = x;
				normalVectors[12 * i + 1] = y;
				normalVectors[12 * i + 2] = z;
				normalVectors[12 * i + 3] = 1.0f;

				normalVectors[12 * i + 4] = x;
				normalVectors[12 * i + 5] = y;
				normalVectors[12 * i + 6] = z;
				normalVectors[12 * i + 7] = 1.0f;

				normalVectors[12 * i + 8] = x;
				normalVectors[12 * i + 9] = y;
				normalVectors[12 * i + 10] = z;
				normalVectors[12 * i + 11] = 1.0f;
			}
			else { // normalVectors for smooth shading

				normalVectors[12 * i + 0] = temp[0];
				normalVectors[12 * i + 1] = temp[1];
				normalVectors[12 * i + 2] = temp[2];
				normalVectors[12 * i + 3] = 1.0f;

				normalVectors[12 * i + 4] = cos(calAngle(i));
				normalVectors[12 * i + 5] = 0.0f;
				normalVectors[12 * i + 6] = sin(calAngle(i));
				normalVectors[12 * i + 7] = 1.0f;

				normalVectors[12 * i + 8] = cos(calAngle(i + 1));
				normalVectors[12 * i + 9] = 0.0f;
				normalVectors[12 * i + 10] = sin(calAngle(i + 1));
				normalVectors[12 * i + 11] = 1.0f;
			}
		}

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(normalVectors), normalVectors);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(colors), colors);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);
	};

};


#endif
