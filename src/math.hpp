#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>
#include <vector>

#include "input.hpp"
#include "camera.hpp"
#include "mesh.hpp"
#include "planet.hpp"
#include "globals.hpp"
#include "gizmos.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

template <typename T> 
inline T Interpolate(T a, T b, float t) {
    return a + t * (b - a);
}

inline int binomialCoeff(int n, int r)
{
    if (r > n)
        return 0;
    long long int m = 1000000007;
    long long int inv[r + 1] = { 0 };
    inv[0] = 1;
    if(r+1>=2)
    inv[1] = 1;
 
    // Getting the modular inversion
    // for all the numbers
    // from 2 to r with respect to m
    // here m = 1000000007
    for (int i = 2; i <= r; i++) {
        inv[i] = m - (m / i) * inv[m % i] % m;
    }
 
    int ans = 1;
 
    // for 1/(r!) part
    for (int i = 2; i <= r; i++) {
        ans = ((ans % m) * (inv[i] % m)) % m;
    }
 
    // for (n)*(n-1)*(n-2)*...*(n-r+1) part
    for (int i = n; i >= (n - r + 1); i--) {
        ans = ((ans % m) * (i % m)) % m;
    }
    return ans;
}

//template <typename T> 
inline glm::vec3 BezierCurve(std::vector<glm::vec3> points, float t) {
    int n = points.size();
    glm::vec3 sum;
    for(int i=0; i<n; i++) {
        sum += binomialCoeff(n-1, i) * static_cast<float>(pow(1.0f-t, n-1-i)) * static_cast<float>(pow(t, i)) * points[i];
    }
    return sum;
}

inline bool MouseRaycast() {
    ImGuiIO &io = ImGui::GetIO();
    if(io.WantCaptureMouse)
        return false;

    float x = (2.0f * cursorX) / scr_width - 1.0f;
    float y = 1.0f - (2.0f * cursorY) / scr_height;
    float z = 1.0f;

    glm::vec4 ray_clip = glm::vec4(x, y, -1.0f, 1.0f);
    glm::vec4 ray_eye = glm::inverse(glm::perspective(glm::radians(85.0f), (float)scr_width / (float)scr_height, 0.01f, 100.0f)) * ray_clip;
    ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0f, 0.0f);
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, mainCam.pos);
    view = glm::lookAt(mainCam.pos, mainCam.pos + mainCam.front, mainCam.up);
    glm::vec3 ray_wor = glm::inverse(view) * ray_eye;
    ray_wor = glm::normalize(ray_wor);
    float ray_length = 10.0f;
    
    for(int i=0; i<planets.size(); i++) {
        glm::vec3 center = planets[i].position;
        glm::vec3 rayStart = mainCam.pos + ray_wor;
        glm::vec3 rayEnd = mainCam.pos + (ray_wor) * ray_length;
        float a = pow(rayEnd.x - rayStart.x, 2) + pow(rayEnd.y - rayStart.y, 2) + pow(rayEnd.z - rayStart.z, 2);
        float b = 2 * ((rayEnd.x - rayStart.x)*(rayStart.x - center.x) + (rayEnd.y - rayStart.y)*(rayStart.y - center.y) + (rayEnd.z - rayStart.z)*(rayStart.z - center.z));
        float c = pow(center.x,2) + pow(center.y,2) + pow(center.z,2) + pow(rayStart.x,2) + pow(rayStart.y,2) + pow(rayStart.z,2) - 2 * (center.x * rayStart.x + center.y * rayStart.y + center.z * rayStart.z) - (planets[i].scale.x * planets[i].scale.x);
        float delta = b*b - 4*a*c;
        float d1 = (-b-sqrt(delta))/(2*a);
        float d2 = (-b+sqrt(delta))/(2*a);
        std::cout << "Delta " << delta
                  << "\nd1 " << d1
                  << "\nd2 " << d2 << "\n";
        if(delta >= 0.0f && (d1 > 0.0f || d2 > 0.0f)) {
            std::cout << "Hit " << planets[i].name << "\n";
            currentPlanet = &planets[i];
            distFromPlanet = planets[i].scale.x * 3.0f;
            return true;
        }
    }
    if(!io.WantCaptureMouse)
        currentPlanet = nullptr;
    return false;
}