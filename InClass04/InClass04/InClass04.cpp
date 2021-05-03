#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <shader.h> // include from utils

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow *window, int key, int scancode, int action , int mods);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

float accumulatedDeltaX = 0.0f, accumulatedDeltaY = 0.0f, delta = 0.2f;
float colorR = 1.0f, colorG = 0.0f, colorB = 0.0f;

int main()
{
    if (!glfwInit())
    {
        printf("GLFW initialisation failed!");
        glfwTerminate();
        return 1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "InClass4: Moving Triangle", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        printf("GLEW initialisation failed!");
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    // shader loading and compile
    Shader ourShader("vertex_shader.vs", "fragment_shader.fs");

    float vertices[] = {
         0.0f,  0.2f, 0.0f,  // top right
        -0.2f, -0.2f, 0.0f,  // bottom right
         0.2f, -0.2f, 0.0f   // bottom left
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 2,  // Triangle indices
    };
    
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!glfwWindowShouldClose(window)) {

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ourShader.use();
        ourShader.setFloat("accumulatedDeltaX", accumulatedDeltaX);
        ourShader.setFloat("accumulatedDeltaY", accumulatedDeltaY);
        ourShader.setVec4("outColor", colorR, colorG, colorB, 1.0);
        
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
        if(0.2f + accumulatedDeltaX <= 0.8f)
            accumulatedDeltaX += delta;
    }
    else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
        if(-0.2f + accumulatedDeltaX >= -0.8f)
            accumulatedDeltaX -= delta;
    }
    else if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
        if (0.2f + accumulatedDeltaY <= 0.8f)
            accumulatedDeltaY += delta;
    }
    else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
        if (-0.2f + accumulatedDeltaY >= -0.8f)
            accumulatedDeltaY -= delta;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}
