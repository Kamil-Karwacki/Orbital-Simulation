#include <iostream>
#include "input.hpp"
#include "globals.hpp"
#include "math.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

void ProcessInput(GLFWwindow* window) {
    ImGuiIO &io = ImGui::GetIO();
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        mainCam.pos += mainCam.speed * mainCam.front * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        mainCam.pos -= mainCam.speed * mainCam.front * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        mainCam.pos -= glm::normalize(glm::cross(mainCam.front, mainCam.up)) * mainCam.speed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        mainCam.pos += glm::normalize(glm::cross(mainCam.front, mainCam.up)) * mainCam.speed * deltaTime;


    if(io.WantCaptureMouse)
        return;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        mainCam.speed = 1.0f;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
        mainCam.speed = 5.0f;
    
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    else
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void MouseCallback(GLFWwindow* window, double xposIn, double yposIn) {
    cursorX = xposIn;
    cursorY = yposIn;
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) != GLFW_PRESS) {
        lastX = xpos;
        lastY = ypos;
        return;
    }
    
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    mainCam.front = glm::normalize(front);
}

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    distFromPlanet -= yoffset;
}