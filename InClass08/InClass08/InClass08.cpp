#define _USE_MATH_DEFINES

#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath>

#include <shader.h>
#include <arcball.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


using namespace std;

static unsigned int texture; // Array of texture ids.

class Cylinder {
public:

	// vertex position array
	GLfloat vertices[60];

	// normal array
	GLfloat normals[60];

	// colour array
	GLfloat colors[80];

	// texture coord array

	GLfloat texCoords[40];

	// index array for glDrawElements()
	// A cube requires 36 indices = 6 sides * 2 tris * 3 verts

	GLuint indices[30];
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;

	unsigned int vSize = sizeof(vertices);
	unsigned int nSize = sizeof(normals);
	unsigned int cSize = sizeof(colors);
	unsigned int tSize = sizeof(texCoords);

	Cylinder() {
		initBuffers();
	};

	void initBuffers() {
		double angle = 2 * M_PI / 5;
		double radius = 1.0f;
		for (int i = 0; i < 5; i++) {
			vertices[12 * i] = radius * cos(angle * i);
			vertices[12 * i + 1] = -1;
			vertices[12 * i + 2] = radius * sin(angle * i);
			vertices[12 * i + 3] = radius * cos(angle * i);
			vertices[12 * i + 4] = 1;
			vertices[12 * i + 5] = radius * sin(angle * i);
			vertices[12 * i + 6] = radius * cos(angle * (i + 1));
			vertices[12 * i + 7] = -1;
			vertices[12 * i + 8] = radius * sin(angle * (i + 1));
			vertices[12 * i + 9] = radius * cos(angle * (i + 1));
			vertices[12 * i + 10] = 1;
			vertices[12 * i + 11] = radius * sin(angle * (i + 1));

			indices[6 * i] = 4 * i;
			indices[6 * i + 1] = 4 * i + 1;
			indices[6 * i + 2] = 4 * i + 2;
			indices[6 * i + 3] = 4 * i + 1;
			indices[6 * i + 4] = 4 * i + 2;
			indices[6 * i + 5] = 4 * i + 3;

			texCoords[8 * i] = 0.2f * i;
			texCoords[8 * i + 1] = 0;
			texCoords[8 * i + 2] = 0.2f * i;
			texCoords[8 * i + 3] = 1;
			texCoords[8 * i + 4] = 0.2f * (i + 1);
			texCoords[8 * i + 5] = 0;
			texCoords[8 * i + 6] = 0.2f * (i + 1);
			texCoords[8 * i + 7] = 1;


		}

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		// copy vertex attrib data to VBO
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vSize + nSize + cSize + tSize, 0, GL_STATIC_DRAW); // reserve space
		glBufferSubData(GL_ARRAY_BUFFER, 0, vSize, vertices);                  // copy verts at offset 0
		glBufferSubData(GL_ARRAY_BUFFER, vSize, nSize, normals);               // copy norms after verts
		glBufferSubData(GL_ARRAY_BUFFER, vSize + nSize, cSize, colors);          // copy cols after norms
		glBufferSubData(GL_ARRAY_BUFFER, vSize + nSize + cSize, tSize, texCoords); // copy texs after cols

		// copy index data to EBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		// attribute position initialization
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);  // position attrib
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)vSize); // normal attrib
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(vSize + nSize)); //color attrib
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(vSize + nSize + cSize)); // tex coord
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	};

	void draw(Shader* shader) {
		shader->use();
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	};
};

// Function Prototypes
GLFWwindow* glAllInit();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double x, double y);
void getTexture();
void render();

// Global variables
GLFWwindow* mainWindow = NULL;
Shader* globalShader = NULL;
unsigned int SCR_WIDTH = 600;
unsigned int SCR_HEIGHT = 600;
Cylinder* cylinder;
glm::mat4 projection, view, model;
glm::vec3 camPosition(0.0f, 3.0f, 7.0f);
glm::vec3 camTarget(0.0f, 0.0f, 0.0f);
glm::vec3 camUp(0.0f, 1.0f, 0.0f);

// for arcball
float arcballSpeed = 0.2f;
static Arcball modelArcBall(SCR_WIDTH, SCR_HEIGHT, arcballSpeed, true, true);

// for texture



int main()
{
	mainWindow = glAllInit();

	// shader loading and compile (by calling the constructor)
	globalShader = new Shader("global.vs", "global.fs");

	// projection and view matrix
	globalShader->use();
	projection = glm::perspective(glm::radians(45.0f),
		(float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	globalShader->setMat4("projection", projection);
	view = glm::lookAt(camPosition, camTarget, camUp);
	globalShader->setMat4("view", view);

	// load texture
	getTexture();

	// create a cube
	cylinder = new Cylinder();

	while (!glfwWindowShouldClose(mainWindow)) {
		render();
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

GLFWwindow* glAllInit()
{
	GLFWwindow* window;

	// glfw: initialize and configure
	if (!glfwInit()) {
		printf("GLFW initialisation failed!");
		glfwTerminate();
		exit(-1);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// glfw window creation
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Inclass08 : Textured Cylinder", NULL, NULL);
	if (window == NULL) {
		cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		exit(-1);
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);

	// OpenGL states
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	// Allow modern extension features
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		cout << "GLEW initialisation failed!" << endl;
		glfwDestroyWindow(window);
		glfwTerminate();
		exit(-1);
	}

	return window;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	else if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		modelArcBall.init(SCR_WIDTH, SCR_HEIGHT, arcballSpeed, true, true);
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	modelArcBall.mouseButtonCallback(window, button, action, mods);
}

void cursor_position_callback(GLFWwindow* window, double x, double y) {
	modelArcBall.cursorCallback(window, x, y);
}

void getTexture() {

	// Create texture ids.
	glGenTextures(1, &texture);

	// All upcomming GL_TEXTURE_2D operations now on "texture" object
	glBindTexture(GL_TEXTURE_2D, texture);

	// Set texture parameters for wrapping.
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Set texture parameters for filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	unsigned char* image = stbi_load("container.bmp", &width, &height, &nrChannels, 0);
	if (!image) {
		printf("texture %s loading error ... \n", "container.bmp");
	}
	else printf("texture %s loaded\n", "container.bmp");

	GLenum format;
	if (nrChannels == 1) format = GL_RED;
	else if (nrChannels == 3) format = GL_RGB;
	else if (nrChannels == 4) format = GL_RGBA;

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

}

void render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	model = modelArcBall.createRotationMatrix();

	globalShader->use();
	globalShader->setMat4("model", model);

	glBindTexture(GL_TEXTURE_2D, texture);

	cylinder->draw(globalShader);

	glfwSwapBuffers(mainWindow);
}