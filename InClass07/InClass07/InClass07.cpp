#define _USE_MATH_DEFINES
#define NUMOFTRIANGLE 32
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath>
#include <shader.h>
#include <arcball.h>



using namespace std;

// types
class ConeObject {
public:
    ConeObject() { };
    ConeObject(Shader* shader) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        this->shader = shader;
        double pi = M_PI;
        double radius = 1.0f;

        for (int i = 0; i <= NUMOFTRIANGLE; ++i) {
            this->v[12 * i] = 0.0f;
            this->v[(12 * i) + 1] = 1.0f;
            this->v[(12 * i) + 2] = 0.0f;
            this->v[(12 * i) + 3] = 1.0f;
            
            double angle = (2 * pi * i)/ NUMOFTRIANGLE;
            this->v[12 * i + 4] = radius * cos(angle);
            this->v[(12 * i) + 5] = -1.0f;
            this->v[(12 * i) + 6] = radius * sin(angle);
            this->v[(12 * i) + 7] = 1.0f;
            
            int j = i + 1;
            angle = (2 * pi * j) / NUMOFTRIANGLE;
            this->v[12 * i + 8] = radius * cos(angle);
            this->v[(12 * i) + 9] = -1.0f;
            this->v[(12 * i) + 10] = radius * sin(angle);
            this->v[(12 * i) + 11] = 1.0f;
        }
    };
    float v[(NUMOFTRIANGLE+1)*12]; // vertex data
    unsigned int VAO;  // vertex array object
    unsigned int VBO;  // vertex buffer object
    Shader* shader;          // shader to be used
};

// Function Prototypes
GLFWwindow *glAllInit();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow *window, int key, int scancode, int action , int mods);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow *window, double x, double y);
void render();

// Global variables
GLFWwindow *mainWindow = NULL;
Shader *globalShader = NULL;
ConeObject* cone = NULL;
unsigned int SCR_WIDTH = 600;
unsigned int SCR_HEIGHT = 600;
glm::mat4 projection, view, model;

// for arcball
float arcballSpeed = 0.2f;
static Arcball camArcBall(SCR_WIDTH, SCR_HEIGHT, arcballSpeed, true, true );
static Arcball modelArcBall(SCR_WIDTH, SCR_HEIGHT, arcballSpeed, true, true);
bool arcballCamRot = true;

// Color
float Colors[] = {
                    0.7f, 0.0f, 0.0f, 1.0f, // red
                    0.0f, 0.7f, 0.0f, 1.0f, // green
                    0.0f, 0.0f, 0.7f, 1.0f, // blue
                    0.7f, 0.7f, 0.0f, 1.0f, // yellow
                    0.0f, 0.7f, 0.7f, 1.0f, // cyan
                    0.7f, 0.0f, 0.7f, 1.0f  // magenta
                 };
int main()
{
    mainWindow = glAllInit();
    
    // shader loading and compile (by calling the constructor)
    globalShader = new Shader("global.vs", "global.fs");
    
    // projection matrix
    globalShader->use();
    projection = glm::perspective(glm::radians(45.0f),
                                  (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    globalShader->setMat4("projection", projection);
    
    // cone initialization
    cone = new ConeObject(globalShader);

    glBindVertexArray(cone->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, cone->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cone->v), cone->v, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    cout << "InClass07: camera rotation mode" << endl;
    
    // render loop
    // -----------
    while (!glfwWindowShouldClose(mainWindow)) {
        render();
        glfwPollEvents();
    }
    
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
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
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "InClass07 Cone", NULL, NULL);
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

void render() {
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    view = glm::lookAt(glm::vec3(0.0f, 3.0f, 7.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                       glm::vec3(0.0f, 1.0f, 0.0f));
    view = view * camArcBall.createRotationMatrix();
    
    globalShader->use();
    globalShader->setMat4("view", view);
    
    cone->shader->use();
    glBindVertexArray(cone->VAO);
    
    for (int i = 0; i <= NUMOFTRIANGLE; ++i) {
        cone->shader->setVec4("toColor", Colors[4 * (i % 6)],Colors[4 * (i % 6) + 1], Colors[4 * (i % 6) + 2], Colors[4 * (i % 6) + 3]);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawArrays(GL_TRIANGLES, 3*i, 3);
    }
    glBindVertexArray(0);

    // center cube
    model = glm::mat4(1.0f);
    model = model * modelArcBall.createRotationMatrix();
    globalShader->setMat4("model", model);
    
    glfwSwapBuffers(mainWindow);
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

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    else if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        camArcBall.init(SCR_WIDTH, SCR_HEIGHT, arcballSpeed, true, true);
        modelArcBall.init(SCR_WIDTH, SCR_HEIGHT, arcballSpeed, true, true);
    }
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    if (arcballCamRot)
        camArcBall.mouseButtonCallback( window, button, action, mods );
    else
        modelArcBall.mouseButtonCallback( window, button, action, mods );
}

void cursor_position_callback(GLFWwindow *window, double x, double y) {
    if (arcballCamRot)
        camArcBall.cursorCallback( window, x, y );
    else
        modelArcBall.cursorCallback( window, x, y );
}
