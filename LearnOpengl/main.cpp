#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <iostream>

#include "main.h"
#include "logger.h"
#include "Shader.h"
#include "Camera.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define WIDTH 1280
#define HEIGHT 720

float deltaTime = 0.0f;
float lastFrame = 0.0f;

std::unique_ptr<Shader> shader;

glm::mat4 model;
glm::mat4 projection;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = WIDTH ;
float lastY = HEIGHT ;
bool firstMouse = true;

struct Vertex
{
	float position[3];
	float texture[2];
	float normal[3];
};

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		error("Failed to create GLFW window");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		error("Failed to initialize GLAD");
		return -1;
	}

	glViewport(0, 0, WIDTH, HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

	shader = std::make_unique<Shader>(Shader("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl"));

	unsigned int texBoxDiffuse = loadTexture("res/box_diffuse.png");
	unsigned int texBoxReflect = loadTexture("res/box_reflect.png");

	glEnable(GL_DEPTH_TEST);

	unsigned int cubeVAO = createCube();

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texBoxDiffuse);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texBoxReflect);

		shader->use();
		shader->setMat4("view", camera.GetViewMatrix());
		shader->setMat4("projection", projection);

		// directional light
		shader->setInt("lightSources[0].isDirLight", 1);
		shader->setVec3("lightSources[0].direction", -0.2f, -1.0f, -0.3f);
		shader->setVec3("lightSources[0].ambient", 0.05f, 0.05f, 0.05f);
		shader->setVec3("lightSources[0].diffuse", 0.4f, 0.4f, 0.4f);
		shader->setVec3("lightSources[0].specular", 0.5f, 0.5f, 0.5f);
		// point light 1
		shader->setInt("lightSources[1].isDirLight", 0);
		shader->setVec3("lightSources[1].position", 0, 0, 2);
		shader->setVec3("lightSources[1].direction", 0, 0, -2);
		shader->setVec3("lightSources[1].ambient", 0.05f, 0.05f, 0.05f);
		shader->setVec3("lightSources[1].diffuse", 0.8f, 0.8f, 0.8f);
		shader->setVec3("lightSources[1].specular", 1.0f, 1.0f, 1.0f);
		shader->setFloat("lightSources[1].constant", 1.0f);
		shader->setFloat("lightSources[1].linear", 0.09f);
		shader->setFloat("lightSources[1].quadratic", 0.032f);
		shader->setFloat("lightSources[1].spotCutoff", 15);

		shader->setInt("enabledLightSourceCount", 2);

		shader->setVec3("viewPos", camera.Position);
		shader->setInt("material.diffuse", 0);
		shader->setInt("material.specular", 1);	
		shader->setFloat("material.shininess", 32.0f);

		glm::mat4 model = glm::mat4(1.0f);
		shader->setMat4("model", model);

		drawCube(cubeVAO);

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}


void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	camera.ProcessMouseMovement(0.0f, 0.0f);
	camera.ProcessMouseScroll(0.0f);
}

GLuint loadTexture(std::string name) {
	int width, height, channels;
	unsigned char* data = stbi_load(name.c_str(), &width, &height, &channels, 0);

	GLuint texture;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	stbi_set_flip_vertically_on_load(true);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, channels == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(data);

	return texture;
}

unsigned int createCube()
{
	Vertex vertices[] = {
			// Back face
			{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
			{{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
			{{0.5f,  0.5f, -0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
			{{0.5f,  0.5f, -0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
			{{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
			{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},

			// Front face
			{{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
			{{0.5f, -0.5f,  0.5f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
			{{0.5f,  0.5f,  0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
			{{0.5f,  0.5f,  0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
			{{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
			{{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},

			// Left face
			{{-0.5f,  0.5f,  0.5f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
			{{-0.5f,  0.5f, -0.5f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
			{{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
			{{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
			{{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
			{{-0.5f,  0.5f,  0.5f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},

			// Right face
			{{0.5f,  0.5f,  0.5f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f,  0.5f, -0.5f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f,  0.5f,  0.5f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},

			// Bottom face
			{{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
			{{0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
			{{0.5f, -0.5f,  0.5f}, {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
			{{0.5f, -0.5f,  0.5f}, {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
			{{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
			{{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},

			// Top face
			{{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
			{{0.5f,  0.5f, -0.5f}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
			{{0.5f,  0.5f,  0.5f}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
			{{0.5f,  0.5f,  0.5f}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}}
	};
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return VAO;
}

void drawCube(unsigned int VAO)
{
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; 
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}