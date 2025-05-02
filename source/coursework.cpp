#include <iostream>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/maths.hpp>
#include <common/camera.hpp>
#include <common/model.hpp>
#include <common/light.hpp>

// Function prototypes
void keyboardInput(GLFWwindow* window);
void mouseInput(GLFWwindow* window);

// Frame timers
float previousTime = 0.0f;  // time of previous iteration of the loop
float deltaTime = 0.0f;  // time elapsed since the previous frame
float rotationSpeed = 20.0f; // degrees per second
// Create camera object
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f));

// Object struct
struct Object
{
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 rotation = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
    float angle = 0.0f;
    std::string name;
};

int main(void)
{
    // =========================================================================
    // Window creation - you shouldn't need to change this code
    // -------------------------------------------------------------------------
    // Initialise GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    GLFWwindow* window;
    window = glfwCreateWindow(1024, 768, "Computer Graphics Coursework Luca", NULL, NULL);

    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window.\n");
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }
    // -------------------------------------------------------------------------
    // End of window creation
    // =========================================================================

    // Enable depth test
    glEnable(GL_DEPTH_TEST);

    // Use back face culling
    glEnable(GL_CULL_FACE);

    // Ensure we can capture keyboard inputs
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Capture mouse inputs
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwPollEvents();
    glfwSetCursorPos(window, 1024 / 2, 768 / 2);

    // Compile shader program
    unsigned int shaderID, lightShaderID;
    shaderID = LoadShaders("vertexShader.glsl", "fragmentShader.glsl");
    lightShaderID = LoadShaders("lightVertexShader.glsl", "lightFragmentShader.glsl");

    // Activate shader
    glUseProgram(shaderID);

    // Load models
    Model teapot("../assets/teapot.obj");
    Model sphere("../assets/sphere.obj");

    // Load the textures
    teapot.addTexture("../assets/blue.bmp", "diffuse");
    teapot.addTexture("../assets/diamond_normal.png", "normal");

    // Define teapot object lighting properties
    teapot.ka = 0.2f;
    teapot.kd = 0.7f;
    teapot.ks = 1.0f;
    teapot.Ns = 20.0f;

    // Add light sources
    Light lightSources;
    lightSources.addPointLight(glm::vec3(2.0f, 2.0f, 2.0f),         // position white star 
        glm::vec3(1.0f, 1.0f, 1.0f),         // colour
        0.1f, 0.1f, 0.1f);                  // attenuation

    lightSources.addPointLight(glm::vec3(1.0f, 1.0f, -8.0f),        // position  green blue star 
        glm::vec3(0.0f, 1.0f, 1.0f),         // colour      
        0.1f, 0.1f, 0.1f);                  // attenuation

    lightSources.addSpotLight(glm::vec3(0.0f, 3.0f, 0.0f),          // position white star 
        glm::vec3(0.0f, -1.0f, 0.0f),         // direction
        glm::vec3(1.0f, 1.0f, 1.0f),          // colour
        0.1f, 0.1f, 0.1f,                    // attenuation
        std::cos(Maths::radians(45.0f)));     // cos(phi)

    lightSources.addSpotLight(glm::vec3(2.0f, 4.0f, 3.0f),          // position white star 
        glm::vec3(0.0f, -1.0f, 0.0f),         // direction
        glm::vec3(1.0f, 1.0f, 1.0f),          // colour
        0.1f, 0.1f, 0.1f,                    // attenuation
        std::cos(Maths::radians(45.0f)));     // cos(phi)

    lightSources.addDirectionalLight(glm::vec3(1.0f, 1.0f, 0.0f),  // direction
        glm::vec3(2.0f, 2.0f, 0.0f));  // colour
    lightSources.addPointLight(glm::vec3(-10.0f, 10.0f, -30.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0.2f, 0.05f, 0.01f);
    lightSources.addPointLight(glm::vec3(15.0f, 15.0f, -10.0f), glm::vec3(0.9f, 0.9f, 1.0f), 0.2f, 0.05f, 0.01f);
    lightSources.addPointLight(glm::vec3(5.0f, 10.0f, -20.0f), glm::vec3(1.0f, 1.0f, 0.95f), 0.2f, 0.05f, 0.01f);
    lightSources.addPointLight(glm::vec3(-20.0f, 15.0f, 10.0f), glm::vec3(1.0f, 0.95f, 1.0f), 0.2f, 0.05f, 0.01f);
    lightSources.addPointLight(glm::vec3(25.0f, 20.0f, 15.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0.2f, 0.05f, 0.01f);
    lightSources.addPointLight(glm::vec3(0.0f, 20.0f, -25.0f), glm::vec3(0.95f, 1.0f, 1.0f), 0.2f, 0.05f, 0.01f);
    lightSources.addPointLight(glm::vec3(-30.0f, 22.0f, 5.0f), glm::vec3(1.0f, 1.0f, 0.98f), 0.2f, 0.05f, 0.01f);
    lightSources.addPointLight(glm::vec3(18.0f, 18.0f, -5.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0.2f, 0.05f, 0.01f);
    lightSources.addPointLight(glm::vec3(-12.0f, 33.0f, 12.0f), glm::vec3(0.98f, 0.98f, 1.0f), 0.2f, 0.05f, 0.01f);
    lightSources.addPointLight(glm::vec3(10.0f, 38.0f, -15.0f), glm::vec3(1.0f, 0.99f, 1.0f), 0.2f, 0.05f, 0.01f);

    // Teapot positions
    glm::vec3 teapotPositions[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -10.0f),
        glm::vec3(-3.0f, -2.0f, -3.0f),
        glm::vec3(-4.0f, -2.0f, -8.0f),
        glm::vec3(2.0f,  2.0f, -6.0f),
        glm::vec3(-4.0f,  3.0f, -8.0f),
        glm::vec3(0.0f, -2.0f, -5.0f),
        glm::vec3(4.0f,  2.0f, -4.0f),
        glm::vec3(2.0f,  0.0f, -2.0f),
        glm::vec3(-1.0f,  1.0f, -2.0f),
        glm::vec3(6.0f,  1.0f, -9.0f),
        glm::vec3(-6.0f,  2.0f, -12.0f),
        glm::vec3(3.0f, -1.0f, -7.0f),
        glm::vec3(-2.0f,  3.0f, -5.0f),
        glm::vec3(5.0f,  0.0f, -10.0f),
        glm::vec3(-5.0f,  1.5f, -6.0f),
        glm::vec3(1.5f,  2.5f, -8.5f),
        glm::vec3(3.0f,  4.0f, -11.0f),
        glm::vec3(-3.5f,  0.5f, -9.0f),
        glm::vec3(0.0f,  3.0f, -13.0f),

        glm::vec3(2.0f,  -3.0f, -6.0f),
        glm::vec3(-4.0f,  0.0f, -10.0f),
        glm::vec3(1.0f,   3.5f, -4.0f),
        glm::vec3(-6.0f, -1.0f, -9.0f),
        glm::vec3(3.5f,   2.0f, -12.0f),
    };

    // Add teapots to objects vector
    std::vector<Object> objects;
    Object object;
    object.name = "teapot";
    for (unsigned int i = 0; i < 24; i++)
    {
        object.position = teapotPositions[i];
        object.rotation = glm::vec3(1.0f, 1.0f, 1.0f);
        object.scale = glm::vec3(1.0f, 1.0f, 1.0f);
        object.angle = Maths::radians(20.0f * i);
        objects.push_back(object);
    }

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        // Update timer
        float time = glfwGetTime();
        deltaTime = time - previousTime;
        previousTime = time;

        // Get inputs
        keyboardInput(window);
        mouseInput(window);

        // Clear the window
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Calculate view and projection matrices
        camera.target = camera.eye + camera.front;
        camera.calculateMatrices();

        // Activate shader
        glUseProgram(shaderID);

        // Send light source properties to the shader
        lightSources.toShader(shaderID, camera.view);

        // Loop through objects
        for (unsigned int i = 0; i < static_cast<unsigned int>(objects.size()); i++)
        {
            objects[i].angle += Maths::radians(rotationSpeed * deltaTime);

            // Calculate model matrix
            glm::mat4 translate = Maths::translate(objects[i].position);
            glm::mat4 scale = Maths::scale(objects[i].scale);
            glm::mat4 rotate = Maths::rotate(objects[i].angle, objects[i].rotation);
            glm::mat4 model = translate * rotate * scale;

            // Send the MVP and MV matrices to the vertex shader
            glm::mat4 MV = camera.view * model;
            glm::mat4 MVP = camera.projection * MV;
            glUniformMatrix4fv(glGetUniformLocation(shaderID, "MVP"), 1, GL_FALSE, &MVP[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(shaderID, "MV"), 1, GL_FALSE, &MV[0][0]);

            // Draw the model
            if (objects[i].name == "teapot")
                teapot.draw(shaderID);
        }

        // Draw light sources
        lightSources.draw(lightShaderID, camera.view, camera.projection, sphere);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    teapot.deleteBuffers();
    glDeleteProgram(shaderID);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();
    return 0;
}

void keyboardInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Move the camera using WSAD keys
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.eye += 5.0f * deltaTime * camera.front;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.eye -= 5.0f * deltaTime * camera.front;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.eye -= 5.0f * deltaTime * camera.right;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.eye += 5.0f * deltaTime * camera.right;
}

void mouseInput(GLFWwindow* window)
{
    // Get mouse cursor position and reset to centre
    double xPos, yPos;
    glfwGetCursorPos(window, &xPos, &yPos);
    glfwSetCursorPos(window, 1024 / 2, 768 / 2);

    // Update yaw and pitch angles
    camera.yaw += 0.005f * float(xPos - 1024 / 2);
    camera.pitch += 0.005f * float(768 / 2 - yPos);

    // Calculate camera vectors from the yaw and pitch angles
    camera.calculateCameraVectors();
}

