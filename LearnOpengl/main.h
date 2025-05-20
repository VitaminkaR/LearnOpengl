#pragma once

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow* window);

GLuint loadTexture(std::string name);

unsigned int createCube();

void drawCube(unsigned int VAO);

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void scene();

void menu();
