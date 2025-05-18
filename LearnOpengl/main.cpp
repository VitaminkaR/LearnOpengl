#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <iostream>

#include "main.h"
#include "logger.h"
#include "Shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define WIDTH 1280
#define HEIGHT 720

std::unique_ptr<Shader> shader;

glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;

struct Vertex
{
	float position[3];
	float color[3];
	float texture[2];
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

	view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
	projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

	shader = std::make_unique<Shader>(Shader("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl"));

	unsigned int texture = loadTexture("res/128.bmp");

	glEnable(GL_DEPTH_TEST);

	unsigned int cubeVAO = createCube();

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		shader->use();
		shader->setInt("texture1", 0);
		shader->setMat4("view", view);
		shader->setMat4("projection", projection);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
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
		{{-0.5f, -0.5f, -0.5f},{1.0f, 1.0f, 1.0f},{ 0.0f, 0.0f}},
		 {{0.5f, -0.5f, -0.5f},{1.0f, 1.0f, 1.0f},{ 1.0f, 0.0f}},
		 {{0.5f,  0.5f, -0.5f},{1.0f, 1.0f, 1.0f},{ 1.0f, 1.0f}},
		 {{0.5f,  0.5f, -0.5f},{1.0f, 1.0f, 1.0f},{ 1.0f, 1.0f}},
		{{-0.5f,  0.5f, -0.5f},{1.0f, 1.0f, 1.0f},{ 0.0f, 1.0f}},
		{{-0.5f, -0.5f, -0.5f},{1.0f, 1.0f, 1.0f},{ 0.0f, 0.0f}},

		{{-0.5f, -0.5f,  0.5f},{1.0f, 1.0f, 1.0f},{ 0.0f, 0.0f}},
		 {{0.5f, -0.5f,  0.5f},{1.0f, 1.0f, 1.0f},{ 1.0f, 0.0f}},
		 {{0.5f,  0.5f,  0.5f},{1.0f, 1.0f, 1.0f},{ 1.0f, 1.0f}},
		 {{0.5f,  0.5f,  0.5f},{1.0f, 1.0f, 1.0f},{ 1.0f, 1.0f}},
		{{-0.5f,  0.5f,  0.5f},{1.0f, 1.0f, 1.0f},{ 0.0f, 1.0f}},
		{{-0.5f, -0.5f,  0.5f},{1.0f, 1.0f, 1.0f},{ 0.0f, 0.0f}},

		{{-0.5f,  0.5f,  0.5f},{1.0f, 1.0f, 1.0f},{ 1.0f, 0.0f}},
		{{-0.5f,  0.5f, -0.5f},{1.0f, 1.0f, 1.0f},{ 1.0f, 1.0f}},
		{{-0.5f, -0.5f, -0.5f},{1.0f, 1.0f, 1.0f},{ 0.0f, 1.0f}},
		{{-0.5f, -0.5f, -0.5f},{1.0f, 1.0f, 1.0f},{ 0.0f, 1.0f}},
		{{-0.5f, -0.5f,  0.5f},{1.0f, 1.0f, 1.0f},{ 0.0f, 0.0f}},
		{{-0.5f,  0.5f,  0.5f},{1.0f, 1.0f, 1.0f},{ 1.0f, 0.0f}},

		 {{0.5f,  0.5f,  0.5f},{1.0f, 1.0f, 1.0f},{ 1.0f, 0.0f}},
		 {{0.5f,  0.5f, -0.5f},{1.0f, 1.0f, 1.0f},{ 1.0f, 1.0f}},
		 {{0.5f, -0.5f, -0.5f},{1.0f, 1.0f, 1.0f},{ 0.0f, 1.0f}},
		 {{0.5f, -0.5f, -0.5f},{1.0f, 1.0f, 1.0f},{ 0.0f, 1.0f}},
		 {{0.5f, -0.5f,  0.5f},{1.0f, 1.0f, 1.0f},{ 0.0f, 0.0f}},
		 {{0.5f,  0.5f,  0.5f},{1.0f, 1.0f, 1.0f},{ 1.0f, 0.0f}},

		{{-0.5f, -0.5f, -0.5f},{1.0f, 1.0f, 1.0f},{ 0.0f, 1.0f}},
		 {{0.5f, -0.5f, -0.5f},{1.0f, 1.0f, 1.0f},{ 1.0f, 1.0f}},
		 {{0.5f, -0.5f,  0.5f},{1.0f, 1.0f, 1.0f},{ 1.0f, 0.0f}},
		 {{0.5f, -0.5f,  0.5f},{1.0f, 1.0f, 1.0f},{ 1.0f, 0.0f}},
		{{-0.5f, -0.5f,  0.5f},{1.0f, 1.0f, 1.0f},{ 0.0f, 0.0f}},
		{{-0.5f, -0.5f, -0.5f},{1.0f, 1.0f, 1.0f},{ 0.0f, 1.0f}},

		{{-0.5f,  0.5f, -0.5f},{1.0f, 1.0f, 1.0f},{ 0.0f, 1.0f}},
		 {{0.5f,  0.5f, -0.5f},{1.0f, 1.0f, 1.0f},{ 1.0f, 1.0f}},
		 {{0.5f,  0.5f,  0.5f},{1.0f, 1.0f, 1.0f},{ 1.0f, 0.0f}},
		 {{0.5f,  0.5f,  0.5f},{1.0f, 1.0f, 1.0f},{ 1.0f, 0.0f}},
		{{-0.5f,  0.5f,  0.5f},{1.0f, 1.0f, 1.0f},{ 0.0f, 0.0f}},
		{{-0.5f,  0.5f, -0.5f},{1.0f, 1.0f, 1.0f},{ 0.0f, 1.0f}}
	};
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
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