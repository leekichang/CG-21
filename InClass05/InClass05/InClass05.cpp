#define _USE_MATH_DEFINES
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <shader.h>

using namespace std;

// types
class CircleObject {
public:
    CircleObject() { };
    CircleObject(Shader *shader) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        this->shader = shader;
        double pi = M_PI;
        double radius = 0.5f;
        for (int i = 0; i < 360; ++i) {
            double angle = i * pi / 180;
            this->v[2 * i] = radius * cos(angle);
            this->v[(2 * i) + 1] = 0.25f + radius * sin(angle);
        }
    };
    float v[720]; // vertex data
    unsigned int VAO;  // vertex array object
    unsigned int VBO;  // vertex buffer object
    Shader *shader;          // shader to be used
};

class LineObject {
public:
    LineObject() { };
    LineObject(Shader* shader) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        this->shader = shader;
        this->v[0] = 0.0f;
        this->v[1] = 0.0f;
        this->v[2] = 0.85f;
        this->v[3] = 0.5f;
    };
    float v[4]; // vertex data
    unsigned int VAO;  // vertex array object
    unsigned int VBO;  // vertex buffer object
    Shader* shader;          // shader to be used
};

class InterObject {
public:
    InterObject() { };
    InterObject(Shader* shader) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        this->shader = shader;
    };
    void setV(float x, float y) {
        v[0] = x;
        v[1] = y;
    }
    float v[2]; // vertex data
    int nInter;
    unsigned int VAO;  // vertex array object
    unsigned int VBO;  // vertex buffer object
    Shader* shader;          // shader to be used
};

// function prototypes
GLFWwindow *glAllInit();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void compute_intersection();
void render();

// global variables
GLFWwindow *window = NULL;
Shader *globalShader = NULL;
CircleObject *circle = NULL;
LineObject *line = NULL;
InterObject* interSection = NULL;
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 800;
unsigned int interVAO;
unsigned int interVBO;
int nInter = 0;
float interV[2] = { 0.0f, };

int main()
{
    window = glAllInit();

    
    globalShader = new Shader("vertex_shader.vs", "fragment_shader.fs");
    
    circle = new CircleObject(globalShader);
    line = new LineObject(globalShader);
    interSection = new InterObject(globalShader);

    glBindVertexArray(circle->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, circle->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(circle->v), circle->v, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glBindVertexArray(line->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, line->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(line->v), line->v, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    compute_intersection();

    glBindVertexArray(interSection->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, interSection->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(interSection->v), interSection->v, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // render loop
    while (!glfwWindowShouldClose(window)) {
        
        render();
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

GLFWwindow *glAllInit()
{
    GLFWwindow *window;
    
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
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "InClass05:Circle-Line Segment Intersection", NULL, NULL);
    if (window == NULL) {
        cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);

    // set some OpenGL states (for transparency and background color)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    
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

void render() {
    
    glClear(GL_COLOR_BUFFER_BIT);
    circle->shader->use();
    glBindVertexArray(circle->VAO);
    circle->shader->setVec4("inColor", 1.0f, 0.0f, 0.0f, 1.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawArrays(GL_LINE_LOOP, 0, 360);
    glBindVertexArray(0);

    line->shader->use();
    glBindVertexArray(line->VAO);
    line->shader->setVec4("inColor", 0.0f, 1.0f, 0.0f, 1.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawArrays(GL_LINE_LOOP, 0, 2);
    glBindVertexArray(0);

    if (interSection->nInter > 0) {
        glPointSize(15.0f);
        globalShader->use();
        glBindVertexArray(interSection->VAO);
        globalShader->setVec4("inColor", 1.0f, 1.0f, 0.0f, 1.0f);
        glDrawArrays(GL_POINTS, 0, interSection->nInter);
        glBindVertexArray(0);
    }
    
    glfwSwapBuffers(window);
}

void compute_intersection() {
    //(0, 0)~(0.85, 0.5) -> y = (0.5/0.85) * x -> (0.5/0.85) * x - y = 0
    float a = (line->v[3] - line->v[1]) / (line->v[2] - line->v[0]),
          b = -1,
          c = b * line->v[1] - a * line->v[0];
    
    float t;
    for (int i = 0; i < 359; i++) {
        float x1 = circle->v[2 * i],
              y1 = circle->v[2 * i + 1],
              x2 = circle->v[2 * i + 2],
              y2 = circle->v[2 * i + 3];

        t = -1 * (a * x1 + b * y1 + c) / (a * (x2 - x1) + b * (y2 - y1));

        if (0.0f <= t && t <= 1.0f) {
            float temp_x = (1 - t) * x1 + t * x2,
                  temp_y = (1 - t) * y1 + t * y2;
            if (line->v[0] <= temp_x && temp_x <= line->v[2] && line->v[1] <= temp_y && temp_y <= line->v[3]) {
                interSection->setV(temp_x, temp_y);
                interSection->nInter += 1;
            }
        }
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}