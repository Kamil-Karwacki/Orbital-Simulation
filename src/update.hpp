#pragma once
#include <glad/glad.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>
#include <algorithm>
#include <iterator>

#include "init.hpp"
#include "input.hpp"
#include "globals.hpp"
#include "mesh.hpp"
#include "planet.hpp"
#include "gizmos.hpp"
#include "shader.hpp"
#include "ui.hpp"


void Update() {
    shader1.Use();
    if(ui_create.createPlanet)
        DrawPlanet(*ui_create.previewPlanet, shader1.program);
        
    projection = glm::perspective(glm::radians(fov), (float)scr_width / (float)scr_height, 0.01f, 100000.0f);
    glUniformMatrix4fv(glGetUniformLocation(shader1.program, "projection"), 1, GL_FALSE, &projection[0][0]);

    view = glm::translate(view, mainCam.pos);
    view = glm::lookAt(mainCam.pos, mainCam.pos + mainCam.front, glm::vec3(0,1,0));
    glUniformMatrix4fv(glGetUniformLocation(shader1.program, "view"), 1, GL_FALSE, &view[0][0]);
    glUniform3f(glGetUniformLocation(shader1.program, "viewPos"), mainCam.pos.x, mainCam.pos.y, mainCam.pos.z);


    for(int i=0; i<planets.size(); i++) { // current planet
        for(int j=0; j<planets.size(); j++) { // other planet
            UpdatePlanetsVel(planets[i], planets[j]);
        }
        planets[i].position += planets[i].vel * deltaTime * timeScale;
    }
    
    std::vector<Planet> previewPlanets = planets;

    for(int i=0; i<previewPlanets.size(); i++) {
        if(previewPlanets[i].name == "") {
            previewPlanets.erase(previewPlanets.begin() + i);
            break;
        }
    }

    shader2.Use();
    glUniformMatrix4fv(glGetUniformLocation(shader2.program, "projection"), 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader2.program, "view"), 1, GL_FALSE, &view[0][0]);
    for(int k=0; k<predictionDepth; k++) {
        for(int i=0; i<previewPlanets.size(); i++) { // current planet
            for(int j=i; j<previewPlanets.size(); j++) { // other planet
                UpdatePlanetsVel2(previewPlanets[i], previewPlanets[j], previewTimeStep);
            }
            previewPlanets[i].position += previewPlanets[i].vel * previewTimeStep;
        }

        for(int i=0; i<previewPlanets.size(); i++) {
            predictMesh[i].position = previewPlanets[i].position;
            predictMesh[i].scale = glm::vec3(0.065f - (((0.065f-0.01)/predictionDepth) * k));
            DrawMesh(predictMesh[i], shader2.program);
        }
    }

    for(int i=0; i<sizeof(stars)/sizeof(Mesh); i++) {
        stars[i].position = mainCam.pos + starsPos[i];
        DrawMesh(stars[i], shader2.program);
    }


    // light calculations //
    shader1.Use();
    int n = 0;
    for(int i=0; i<planets.size(); i++) {
        if(planets[i].name == "")
            continue;

        std::string num = std::to_string(i);
        if(planets[i].emissionStrength > 0.0f) {
            shader1.Use();
            glUniform3f(glGetUniformLocation(shader1.program, std::string("light["+num+"].col").c_str()), planets[i].emissionCol.x, planets[i].emissionCol.y, planets[i].emissionCol.z);
            glUniform3f(glGetUniformLocation(shader1.program, std::string("light["+num+"].pos").c_str()), planets[i].position.x, planets[i].position.y, planets[i].position.z);
            glUniform1f(glGetUniformLocation(shader1.program, std::string("light["+num+"].constant").c_str()), 1.0f);
            glUniform1f(glGetUniformLocation(shader1.program, std::string("light["+num+"].linear").c_str()), 0.009f/planets[i].emissionStrength);
            glUniform1f(glGetUniformLocation(shader1.program, std::string("light["+num+"].quadratic").c_str()), 0.0032f/(planets[i].emissionStrength * 10));
            glUniform1f(glGetUniformLocation(shader1.program, std::string("light["+num+"].radius").c_str()), planets[i].scale.x);
            glUniform1f(glGetUniformLocation(shader1.program, std::string("light["+num+"].strength").c_str()), 1.0f);
            n++;
            shader2.Use();
            DrawPlanet(planets[i], shader2.program);
        }
        if(planets[i].emissionStrength <= 0.0f) {
            shader1.Use();
            glUniform1f(glGetUniformLocation(shader1.program, std::string("light["+num+"].strength").c_str()), 0.0f);

            DrawPlanet(planets[i], shader1.program);
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    bool horizontal = true, first_iteration = true;
    shader4.Use();
    for (int i=0; i<bloomPasses; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
        glUniform1i(glGetUniformLocation(shader4.program, "horizontal"), horizontal);
        glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorbuffers[!horizontal]);
        
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);

        horizontal = !horizontal;
        if (first_iteration)
            first_iteration = false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shader5.Use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
    glUniform1i(glGetUniformLocation(shader5.program, "bloom"), bloom);
    glUniform1f(glGetUniformLocation(shader5.program, "exposure"), exposure);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    DrawUI();
}