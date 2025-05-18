#pragma once

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow* window);

GLuint loadTexture(std::string name);
