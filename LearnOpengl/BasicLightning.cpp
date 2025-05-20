#include "BasicLightning.h"

BasicLightning::BasicLightning()
{
}

void BasicLightning::init()
{
	m_Shader = std::make_unique<Shader>(Shader("shaders/basic_lightningvs.glsl", "shaders/basic_lightningfs.glsl"));

	m_Shader->use();
	m_Shader->setInt("material.diffuse", 0);
	m_Shader->setInt("material.specular", 1);
	m_Shader->setFloat("material.shininess", 32.0f);
}

void BasicLightning::use(Camera camera, glm::mat4 proj)
{
	m_Shader->use();

	m_Shader->setMat4("view", camera.GetViewMatrix());
	m_Shader->setMat4("projection", proj);
	m_Shader->setVec3("viewPos", camera.Position);
}

void BasicLightning::setModelMat(glm::mat4 model)
{
	m_Shader->setMat4("model", model);
}

void BasicLightning::addLightSource(LightSource ls)
{
	if (m_Lights.size() == MAX_LIGHT_SOURCE)
	{
		LOGL::warning("void BasicLightning::addLightSource(LightSource ls) -> max light source == %d", MAX_LIGHT_SOURCE);
		return;
	}

	m_Lights.push_back(ls);
	editLightSource(m_Lights.size() - 1, ls);
}

void BasicLightning::editLightSource(int ID, LightSource ls)
{
	if (ID < 0 && ID > MAX_LIGHT_SOURCE)
	{
		LOGL::error("void BasicLightning::editLightSource(int ID, LightSource ls) -> Wrong ID");
		return;
	}

	m_Shader->use();
	std::string lsname = "lightSources[" + std::to_string(ID) + "].";

	m_Shader->setVec3(lsname + "direction", ls.direction);
	m_Shader->setVec3(lsname + "ambient", ls.ambient);
	m_Shader->setVec3(lsname + "diffuse", ls.diffuse);
	m_Shader->setVec3(lsname + "specular", ls.specular);

	if (ls.isDirLight)
	{
		m_Shader->setInt(lsname + "isDirLight", 1);
	}
	else
	{
		m_Shader->setInt(lsname + "isDirLight", 0);
		m_Shader->setVec3(lsname + "position", ls.position);
		m_Shader->setFloat(lsname + "constant", ls.constant);
		m_Shader->setFloat(lsname + "linear", ls.linear);
		m_Shader->setFloat(lsname + "quadratic", ls.quadratic);
		m_Shader->setFloat(lsname + "spotCutoff", ls.spotCutoff);
	}

	m_Shader->setInt("enabledLightSourceCount", m_Lights.size());
}

void BasicLightning::removeLightSource(int ID)
{
	if (ID < 0 && ID >= m_Lights.size())
	{
		LOGL::error("void BasicLightning::removeLightSource(int ID) -> Wrong ID");
		return;
	}

	if (m_Lights.size() == 1 || ID == MAX_LIGHT_SOURCE - 1)
	{
		m_Lights.pop_back();
		LightSource ls = {};
		editLightSource(ID, ls);
		return;
	}

	m_Lights.at(ID) = m_Lights.at(m_Lights.size() - 1);
	m_Lights.pop_back();

	LightSource ls = {};
	editLightSource(m_Lights.size() - 1, ls);
	editLightSource(ID, m_Lights.at(ID));
}

LightSource* BasicLightning::getLightSource(int ID)
{
	if (ID < 0 && ID >= m_Lights.size())
	{
		LOGL::error("LightSource* BasicLightning::getLightSource(int ID) -> Wrong ID");
		return nullptr;
	}
	return &m_Lights.at(ID);
}
