#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <iostream>

#include "main.h"
#include "logger.h"
#include "BasicLightning.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define WIDTH 1280
#define HEIGHT 720

float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::mat4 model;
glm::mat4 projection;

LOGL::Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = WIDTH;
float lastY = HEIGHT;
bool firstMouse = true;

struct Vertex
{
	float position[3];
	float texture[2];
	float normal[3];
};

bool isMenuOpened = false;

LOGL::BasicLightning basicLightning;
unsigned int texBoxDiffuse;
unsigned int texBoxReflect;
unsigned int cubeVAO;

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		LOGL::error("Failed to create GLFW window");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		LOGL::error("Failed to initialize GLAD");
		return -1;
	}

	glViewport(0, 0, WIDTH, HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

	basicLightning.init();
	LOGL::LightSource dirls;
	dirls.isDirLight = true;
	dirls.direction = glm::vec3(0.0f, -1.0f, 0.0f);
	dirls.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
	dirls.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	dirls.specular = glm::vec3(1.0f, 1.0f, 1.0f);
	basicLightning.addLightSource(dirls);
	dirls.isDirLight = false;
	dirls.position = glm::vec3(-1.0f, 1.0f, 1.0f);
	dirls.direction = glm::vec3(0.0f, -1.0f, 0.0f);
	dirls.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
	dirls.diffuse = glm::vec3(1.0f, 0.0f, 0.0f);
	dirls.specular = glm::vec3(1.0f, 1.0f, 1.0f);
	basicLightning.addLightSource(dirls);

	texBoxDiffuse = loadTexture("res/box_diffuse.png");
	texBoxReflect = loadTexture("res/box_reflect.png");

	glEnable(GL_DEPTH_TEST);

	cubeVAO = createCube();

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		if (isMenuOpened)
			menu();

		processInput(window);

		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		scene();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
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

	static bool f1KeyRepeatFlag = false;
	if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS && !f1KeyRepeatFlag)
	{
		f1KeyRepeatFlag = true;
		if (isMenuOpened)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		isMenuOpened = !isMenuOpened;
	}
	if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_RELEASE)
		f1KeyRepeatFlag = false;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(LOGL::FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(LOGL::BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LOGL::LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(LOGL::RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.ProcessKeyboard(LOGL::UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera.ProcessKeyboard(LOGL::DOWN, deltaTime);
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

	if (!isMenuOpened)
		camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void scene()
{
	basicLightning.use(camera, projection);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texBoxDiffuse);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texBoxReflect);
	glm::mat4 model = glm::mat4(1.0f);
	basicLightning.setModelMat(model);
	drawCube(cubeVAO);
}

void menu()
{
	ImGui::ShowDemoWindow();
}