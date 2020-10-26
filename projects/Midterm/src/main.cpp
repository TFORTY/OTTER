#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream> 
#include <string> 

#include <GLM/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <GLM/gtc/type_ptr.hpp>

//For textures
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

GLFWwindow* window;

bool initGLFW() {
	if (glfwInit() == GLFW_FALSE) {
		std::cout << "Failed to Initialize GLFW" << std::endl;
		return false;
	}

	//Create a new GLFW window
	window = glfwCreateWindow(1000, 800, "CG Midterm", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	return true;
}

bool initGLAD() {
	if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0) {
		std::cout << "Failed to initialize Glad" << std::endl;
		return false;
	}
}


int main()
{
	//Initialize GLFW
	if (!initGLFW())
		return 1;

	//Initialize GLAD
	if (!initGLAD())
		return 1;

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		glClearColor(0.08f, 0.17f, 0.31f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glfwSwapBuffers(window);
	}

	return 0;
}