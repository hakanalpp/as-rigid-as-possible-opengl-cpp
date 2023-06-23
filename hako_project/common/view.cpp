#include <iostream>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../src/mesh.h"

using namespace glm;

class View {
private:
    Mesh* mesh;
    GLFWwindow* window;
    glm::mat4 ViewMatrix;
    glm::mat4 ProjectionMatrix;

    glm::vec3 position;
    float horizontalAngle;
    float verticalAngle;
    float mouseSpeed;
    float scrollUpOrDown;
    float moveUpOrDown;
    float moveLeftOrRight;
    bool isLeftClickPressed;
    bool isFirstLeftClickMovement;
    bool isRightClickPressed;
    bool isFirstRightClickMovement;
    double previousLeftMouseX;
    double previousLeftMouseY;
    double previousRightMouseX;
    double previousRightMouseY;

public:
    View(GLFWwindow* win, Mesh* mesh) : window(win) {
        this->mesh = mesh;
        position = glm::vec3(0, 0, 5);
        horizontalAngle = 3.14f;
        verticalAngle = 0.0f;
        mouseSpeed = 0.001f;
        scrollUpOrDown = 0;
        moveUpOrDown = 0;
        moveLeftOrRight = 0;
        isLeftClickPressed = false;
        isFirstLeftClickMovement = true;
        isRightClickPressed = false;
        isFirstRightClickMovement = true;
        previousLeftMouseX = 0;
        previousLeftMouseY = 0;
        previousRightMouseX = 0;
        previousRightMouseY = 0;
    }

    glm::mat4 getViewMatrix() {
        return ViewMatrix;
    }

    glm::mat4 getProjectionMatrix() {
        return ProjectionMatrix;
    }

    void scrollCallback(double xoffset, double yoffset) {
        if (yoffset == 0) {
            scrollUpOrDown = 0.0;
        }
        scrollUpOrDown = yoffset > 0 ? -1.0 : 1.0;
    }

    void mouseButtonCallback(int button, int action, int mods) {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            if (action == GLFW_PRESS) {
                isLeftClickPressed = true;
                isFirstLeftClickMovement = true;
                glfwGetCursorPos(window, &previousLeftMouseX, nullptr);
            }
            else if (action == GLFW_RELEASE) {
                isLeftClickPressed = false;
            }
        }

        if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            if (action == GLFW_PRESS) {
                isRightClickPressed = true;
                isFirstRightClickMovement = true;
                glfwGetCursorPos(window, &previousLeftMouseX, nullptr);
            }
            else if (action == GLFW_RELEASE) {
                isRightClickPressed = false;
            }
        }
    }

    void cursorPosCallback(double xpos, double ypos) {
        if (isFirstRightClickMovement) {
            previousRightMouseX = xpos;
            previousRightMouseY = ypos;
            isFirstRightClickMovement = false;
            return;
        }

        if (isFirstLeftClickMovement) {
            previousLeftMouseX = xpos;
            previousLeftMouseY = ypos;
            isFirstLeftClickMovement = false;
            return;
        }

        if (isLeftClickPressed) {
            int nearestID = findNearestVertexID(xpos, ypos);
            mesh->vertex_types[nearestID] = 1;
            // std::cout<<mesh->vertex_types[5]<<std::endl;
            // for (int i = 0; i < mesh->vertex_types.size(); i++) {
            //     mesh->vertex_types[i] = 1.0;
            // }

            float deltaX = static_cast<float>(previousLeftMouseX - xpos);
            float deltaY = static_cast<float>(previousLeftMouseY - ypos);
            horizontalAngle += deltaX * mouseSpeed;
            verticalAngle += deltaY * mouseSpeed;

            previousLeftMouseX = xpos;
            previousLeftMouseY = ypos;
        }

        if (isRightClickPressed) {
            float deltaX = static_cast<float>(previousRightMouseX - xpos);
            float deltaY = static_cast<float>(ypos - previousRightMouseY);

            moveLeftOrRight = deltaX * mouseSpeed * 10;
            moveUpOrDown = deltaY * mouseSpeed * 10;

            previousRightMouseX = xpos;
            previousRightMouseY = ypos;
        }
    }

    void computeMatricesFromInputs() {
        // glfwGetTime is called only once, the first time this function is called
        static double lastTime = glfwGetTime();

        // Compute time difference between current and last frame
        double currentTime = glfwGetTime();
        float deltaTime = float(currentTime - lastTime);

        glfwSetMouseButtonCallback(window, [](GLFWwindow* win, int button, int action, int mods) {
            View* view = static_cast<View*>(glfwGetWindowUserPointer(win));
            view->mouseButtonCallback(button, action, mods);
        });

        glfwSetCursorPosCallback(window, [](GLFWwindow* win, double xpos, double ypos) {
            View* view = static_cast<View*>(glfwGetWindowUserPointer(win));
            view->cursorPosCallback(xpos, ypos);
        });

        glfwSetScrollCallback(window, [](GLFWwindow* win, double xoffset, double yoffset) {
            View* view = static_cast<View*>(glfwGetWindowUserPointer(win));
            view->scrollCallback(xoffset, yoffset);
        });

        // Direction: Spherical coordinates to Cartesian coordinates conversion
        glm::vec3 direction(
            cos(verticalAngle) * sin(horizontalAngle),
            sin(verticalAngle),
            cos(verticalAngle) * cos(horizontalAngle));

        // Right vector
        glm::vec3 right = glm::vec3(
            sin(horizontalAngle - 3.14f / 2.0f),
            0,
            cos(horizontalAngle - 3.14f / 2.0f));

        // Up vector
        glm::vec3 up = glm::cross(right, direction);

        position += direction * deltaTime * scrollUpOrDown * 50.0f;
        position += up * moveUpOrDown;
        position += right * moveLeftOrRight;
        moveLeftOrRight = 0.0f;
        moveUpOrDown = 0.0f;
        scrollUpOrDown = 0.0f;

        // Projection matrix: 45° Field of View, 4:3 ratio, display range: 0.1 unit <-> 100 units
        ProjectionMatrix = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);

        // Camera matrix
        ViewMatrix = glm::lookAt(
            position,                   // Camera is here
            position + direction,       // and looks here: at the same position, plus "direction"
            up                          // Head is up (set to 0,-1,0 to look upside-down)
        );

        // For the next frame, the "last time" will be "now"
        lastTime = currentTime;
    }

    int findNearestVertexID(double xpos, double ypos) {
        // Get the normalized device coordinates (NDC) from mouse position
        float xNDC = (2.0f * xpos) / 1024 - 1.0f;
        float yNDC = 1.0f - (2.0f * ypos) / 768;

        // Create a ray in view space
        glm::vec4 rayView = glm::inverse(ProjectionMatrix) * glm::vec4(xNDC, yNDC, -1.0f, 1.0f);
        rayView = glm::vec4(rayView.x, rayView.y, -1.0f, 0.0f);

        // Transform the ray to world space
        glm::vec4 rayWorld = glm::inverse(ViewMatrix) * rayView;
        glm::vec3 rayDirection = glm::normalize(glm::vec3(rayWorld));

        // Find the nearest vertex ID
        int nearestID = -1;
        float nearestDistance = std::numeric_limits<float>::max();
        if (mesh == NULL) std::cout << "its NULL" << std::endl;
        // std::cout<<mesh->vertices == nullptr<<std::endl;
        for (int i = 0; i < mesh->vertices.size(); ++i) {
            glm::vec3 vertex = mesh->vertices[i];
            float distance = glm::distance(vertex, position);
            if (distance < nearestDistance) {
                nearestDistance = distance;
                nearestID = i;
            }
        }

        return nearestID;
    }
};
