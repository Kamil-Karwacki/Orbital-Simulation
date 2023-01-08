#include <iostream>
#include <algorithm>
#include "input.hpp"
#include "globals.hpp"
#include "planet.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <glm/gtx/io.hpp>


void ProcessInput(GLFWwindow* window) {
    ImGuiIO &io = ImGui::GetIO();
    if(!io.WantCaptureKeyboard) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            if(currentPlanet || ui_create.createPlanet) {
                float minDistance = currentPlanet ? currentPlanet->scale.x : ui_create.previewPlanet->scale.x;
                minDistance *= 2.0f;
                distFromPlanet = std::clamp(distFromPlanet - deltaTime * distFromPlanet, minDistance, 10000.0f);
            }

            if(!currentPlanet && !ui_create.createPlanet) {
                mainCam.pos += mainCam.speed * mainCam.front * deltaTime;
            }
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            if(currentPlanet || ui_create.createPlanet) {
                float minDistance = currentPlanet ? currentPlanet->scale.x : ui_create.previewPlanet->scale.x;
                minDistance *= 2.0f;
                distFromPlanet = std::clamp(distFromPlanet + deltaTime * distFromPlanet, minDistance, 10000.0f);
            }

            if(!currentPlanet && !ui_create.createPlanet) {
                mainCam.pos -= mainCam.speed * mainCam.front * deltaTime;
            }
        }
        
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && (!currentPlanet && !ui_create.createPlanet))
            mainCam.pos -= glm::normalize(glm::cross(mainCam.front, mainCam.up)) * mainCam.speed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && (!currentPlanet && !ui_create.createPlanet))
            mainCam.pos += glm::normalize(glm::cross(mainCam.front, mainCam.up)) * mainCam.speed * deltaTime;
    }


    

    if(!io.WantCaptureMouse) {
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            mainCam.speed = 1.0f;
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
            mainCam.speed = 5.0f;
        
        if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }


    glm::vec3 point = glm::vec3(0);
    if(currentPlanet)
        point = currentPlanet->position;
    if(ui_create.createPlanet)
        point = ui_create.previewPlanet->position;
    
    if(glm::length(point) > 0) {
        glm::vec3 abc = glm::normalize(mainCam.pos - point);
        mainCam.pos = point + abc * distFromPlanet;
    }
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

    if(currentPlanet) { 
        RotateAroundPoint(currentPlanet->position, xoffset, yoffset);
    } 
    if(ui_create.createPlanet) {
        RotateAroundPoint(ui_create.previewPlanet->position, xoffset, yoffset);
    }
    
    if(!currentPlanet && !ui_create.createPlanet){
        float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        mainCam.front = glm::normalize(front);

        if(yaw > 360) yaw-=360;
        if(yaw < 0) yaw+=360;
    }
    lastX = xpos;
    lastY = ypos;
}

void RotateAroundPoint(glm::vec3 point, float xoffset, float yoffset) {
    // https://asliceofrendering.com/camera/2019/11/30/ArcballCamera/
    glm::vec3 viewDir = glm::normalize(point - mainCam.pos);
    glm::vec3 upVector = glm::vec3(0, 1, 0);
    glm::vec3 right = glm::cross(viewDir, upVector);
    glm::vec4 position(mainCam.pos.x, mainCam.pos.y, mainCam.pos.z, 1);
    glm::vec4 pivot(point.x, point.y, point.z, 1);

    float deltaAngleX = (glm::two_pi<float>() / scr_width);
    float deltaAngleY = (glm::pi<float>() / scr_height);
    float xAngle = xoffset * deltaAngleX;
    float yAngle = yoffset * deltaAngleY;

    float cosAngle = glm::dot(point - mainCam.pos, upVector);
    if(cosAngle * glm::sign(yAngle) > 0.99f) 
        deltaAngleY = 0;
    
    glm::mat4x4 rotationMatrixX(1.0f);
    rotationMatrixX = glm::rotate(rotationMatrixX, xAngle, upVector);
    position = (rotationMatrixX * (position - pivot)) + pivot;

    glm::mat4x4 rotationMatrixY(1.0f);
    rotationMatrixY = glm::rotate(rotationMatrixY, yAngle, right);
    position = (rotationMatrixY * (position - pivot)) + pivot;

    mainCam.pos = glm::vec3(position.x, position.y, position.z);
    mainCam.front = glm::normalize(point - glm::vec3(position.x, position.y, position.z));

    // update yaw and pitch to avoid jumping when unfocusing from planet // 
    float tempYaw = 90.0f - glm::degrees(atan2(mainCam.front.x, mainCam.front.z));
    float tempPitch = glm::degrees(atan2(mainCam.front.y, sqrt(mainCam.front.x * mainCam.front.x + mainCam.front.z * mainCam.front.z)));

    if(tempYaw > 360) tempYaw-=360;
    if(tempYaw < 0) tempYaw+=360;

    yaw = tempYaw;
    pitch = tempPitch;
}

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    
}