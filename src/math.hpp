#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>
#include <vector>

#include "input.hpp"
#include "camera.hpp"
#include "planet.hpp"
#include "globals.hpp"

#include "imgui/imgui.h"

template <typename T> 
inline T Interpolate(T a, T b, float t) {
    return a + t * (b - a);
}

inline bool MouseRaycast() {
    ImGuiIO &io = ImGui::GetIO();
    if(io.WantCaptureMouse)
        return false;

    float x = (2.0f * cursorX) / scr_width - 1.0f;
    float y = 1.0f - (2.0f * cursorY) / scr_height;
    float z = 1.0f;

    glm::vec4 ray_clip = glm::vec4(x, y, -1.0f, 1.0f);
    glm::vec4 ray_eye = glm::inverse(glm::perspective(glm::radians(fov), (float)scr_width / (float)scr_height, 0.01f, 1000.0f)) * ray_clip;
    ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0f, 0.0f);
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, mainCam.pos);
    view = glm::lookAt(mainCam.pos, mainCam.pos + mainCam.front, mainCam.up);
    glm::vec3 ray_wor = glm::inverse(view) * ray_eye;
    ray_wor = glm::normalize(ray_wor);
    float ray_length = 10.0f;
    
    for(int i=0; i<planets.size(); i++) {
        if(planets[i].name == "")
            continue;
        glm::vec3 center = planets[i].position;
        glm::vec3 rayStart = mainCam.pos + ray_wor;
        glm::vec3 rayEnd = mainCam.pos + (ray_wor) * ray_length;
        float a = pow(rayEnd.x - rayStart.x, 2) + pow(rayEnd.y - rayStart.y, 2) + pow(rayEnd.z - rayStart.z, 2);
        float b = 2 * ((rayEnd.x - rayStart.x)*(rayStart.x - center.x) + (rayEnd.y - rayStart.y)*(rayStart.y - center.y) + (rayEnd.z - rayStart.z)*(rayStart.z - center.z));
        float c = pow(center.x,2) + pow(center.y,2) + pow(center.z,2) + pow(rayStart.x,2) + pow(rayStart.y,2) + pow(rayStart.z,2) - 2 * (center.x * rayStart.x + center.y * rayStart.y + center.z * rayStart.z) - (planets[i].scale.x * planets[i].scale.x);
        float delta = b*b - 4*a*c;
        float d1 = (-b-sqrt(delta))/(2*a);
        float d2 = (-b+sqrt(delta))/(2*a);
        std::cout << "Delta " << delta << " d1 " << d1 << " d2 " << d2 << "\n" << planets[i].name << "\n\n";
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

// https://adrianb.io/2014/08/09/perlinnoise.html <3 //
inline int repeat = 11;

inline int permutation[] = { 151,160,137,91,90,15,
    131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
    190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
    88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
    77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
    102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
    135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
    5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
    223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
    129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
    251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
    49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
    138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,
    151,160,137,91,90,15
};

inline int p[512];
inline int oldP[512];

inline double Fade(double t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

inline int Inc(int num) {
    num++;
    if (repeat > 0) num %= repeat;
    
    return num;
}

inline double Grad(int hash, double x, double y, double z)
{
    switch(hash & 0xF)
    {
        case 0x0: return  x + y;
        case 0x1: return -x + y;
        case 0x2: return  x - y;
        case 0x3: return -x - y;
        case 0x4: return  x + z;
        case 0x5: return -x + z;
        case 0x6: return  x - z;
        case 0x7: return -x - z;
        case 0x8: return  y + z;
        case 0x9: return -y + z;
        case 0xA: return  y - z;
        case 0xB: return -y - z;
        case 0xC: return  y + x;
        case 0xD: return -y + z;
        case 0xE: return  y - x;
        case 0xF: return -y - z;
        default: return 0;
    }
}

inline double Perlin(double x, double y, double z) {
    if(repeat > 0) {
        x = std::fmod(x, repeat);
        y = std::fmod(y, repeat);
        z = std::fmod(z, repeat);
    }

    int xi = static_cast<int>(x) & 255;
    int yi = static_cast<int>(y) & 255;
    int zi = static_cast<int>(z) & 255;
    
    double xf = x-static_cast<int>(x);
    double yf = y-static_cast<int>(y);
    double zf = z-static_cast<int>(z);

    double u = Fade(xf);
    double v = Fade(yf);
    double w = Fade(zf);

    int aaa, aba, aab, abb, baa, bba, bab, bbb;
    aaa = p[p[p[    xi ]+    yi ]+    zi ];
    aba = p[p[p[    xi ]+Inc(yi)]+    zi ];
    aab = p[p[p[    xi ]+    yi ]+Inc(zi)];
    abb = p[p[p[    xi ]+Inc(yi)]+Inc(zi)];
    baa = p[p[p[Inc(xi)]+    yi ]+    zi ];
    bba = p[p[p[Inc(xi)]+Inc(yi)]+    zi ];
    bab = p[p[p[Inc(xi)]+    yi ]+Inc(zi)];
    bbb = p[p[p[Inc(xi)]+Inc(yi)]+Inc(zi)];

    double x1, x2, y1, y2;
    x1 = Interpolate( Grad(aaa, xf, yf, zf), Grad(baa, xf-1, yf, zf), u);
    x2 = Interpolate( Grad(aba, xf, yf-1, zf), Grad(bba, xf-1, yf-1, zf), u);
    y1 = Interpolate(x1, x2, v);

    x1 = Interpolate( Grad(aab, xf, yf, zf-1), Grad (bab, xf-1, yf, zf-1), u);
    x2 = Interpolate( Grad(abb, xf, yf-1, zf-1), Grad (bbb, xf-1, yf-1, zf-1), u);
    y2 = Interpolate(x1, x2, v);
    
    return (Interpolate (y1, y2, w)+1)/2;  
}

inline double OctavePerlin(double x, double y, double z, int octaves, double persistence) {
    double total = 0;
    double frequency = 1;
    double amplitude = 1;
    double maxValue = 0;
    for(int i=0;i<octaves;i++) {
        total += Perlin(x * frequency, y * frequency, z * frequency) * amplitude;
        
        maxValue += amplitude;
        
        amplitude *= persistence;
        frequency *= 2;
    }
    
    return total/maxValue;
}