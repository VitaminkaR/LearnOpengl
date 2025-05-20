#pragma once

#include "Shader.h"
#include "glm/glm.hpp"
#include <vector>
#include "logger.h"
#include "Camera.h"

#define MAX_LIGHT_SOURCE 8 

struct LightSource
{
    bool isDirLight;

    glm::vec3 position;
    glm::vec3 direction;

    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
    float spotCutoff = 180.0f;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

class BasicLightning
{
public:
	BasicLightning();
	void init();
	void use(Camera camera, glm::mat4 proj);
    void setModelMat(glm::mat4 model);

    void addLightSource(LightSource ls);
    void editLightSource(int ID, LightSource ls);
    void removeLightSource(int ID);
    LightSource* getLightSource(int ID);
private:
    std::unique_ptr<Shader> m_Shader;
    std::vector<LightSource> m_Lights;
};

