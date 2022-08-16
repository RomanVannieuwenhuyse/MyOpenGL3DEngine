#include"imgui.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<iostream>

#include "ShaderClass.h"

#define PI 3.14159265359

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int width, height;

int main() {
	//camera settings
	int FOV = 70;
	float NearClippingPlane = 0.001;
	float FarClippingPlane = 1000;
	float CameraPosition[3] = { 0.0, 0.0, 0.0 };
	float CameraRotation[3] = { 0.0, 0.0, 0.0 };

	//initialize glfw
	glfwInit();
	//Give the windowhints
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//core profile no need for legacy
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//create a window object
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create window" << "\n";
		//need to terminate glfw
		glfwTerminate();
		return -1;
	}
	//set it as our current context
	glfwMakeContextCurrent(window);
	

	//initialize glad before calling opengl functions
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << "\n";
		glfwTerminate();
		return -1;
	}
	//set a callback function to resize the viewport when the window is resized
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	Shader shaderProgram("default.vert", "default.frag");

	float vertices[] = {
		-0.5f, -0.5f, 1.0f,
		0.5f, -0.5f, 1.0f,
		0.5f, 0.5f, 5.0f,
		-0.5f, 0.5f, 1.0f
	};

	unsigned int indices[] = {
		0, 1, 2,
		0, 2, 3
	};

	//setting up VAO, VBO, EBO
	unsigned int VAO, VBO, EBO;
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenVertexArrays(1, &VAO);
	//bind VAO first for it to get associated with the VBO
	glBindVertexArray(VAO);
	//bind VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//copy our vertex data to it
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//bind EBO and copy index data to it
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	//set our vertex attribute pointer
	//location (see vertex shader), size of vertex attribute (vec3 in this case), type, normalize?, stride, starting point(int cast as a void pointer)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//unbind them (first vao, then vbo, ebo)
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
	
	GLint ARLocation = glGetUniformLocation(shaderProgram.ID, "AspectRatio");
	GLint FOVLocation = glGetUniformLocation(shaderProgram.ID, "FOV");
	GLint NCPLocation = glGetUniformLocation(shaderProgram.ID, "NearClippingPlane");
	GLint FCPLocation = glGetUniformLocation(shaderProgram.ID, "FarClippingPlane");
	GLint CPLocation = glGetUniformLocation(shaderProgram.ID, "CPosition");
	GLint CRLocation = glGetUniformLocation(shaderProgram.ID, "CRotation");
	//main loop
	while (!glfwWindowShouldClose(window)) {
		//simple processing inputs function
		processInput(window);

		//rendering commands
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		glfwGetWindowSize(window, &width, &height);
		
		shaderProgram.Activate();
		glUniform1f(ARLocation, width / height);
		glUniform1f(FOVLocation, (FOV * PI / 180)/2);
		glUniform1f(NCPLocation, NearClippingPlane);
		glUniform1f(FCPLocation, FarClippingPlane);
		glUniform3f(CPLocation, CameraPosition[0], CameraPosition[1], CameraPosition[2]);
		glUniform3f(CRLocation, (CameraRotation[0] * PI / 180) / 2, (CameraRotation[1] * PI / 180) / 2, (CameraRotation[2] * PI / 180) / 2);
		

		glBindVertexArray(VAO);
		//primitives, number of vertices, type of EBO, offset
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		ImGui::Begin("Camera Inspector");
		ImGui::DragFloat3("Position", CameraPosition, 0.01, -std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
		ImGui::DragFloat3("Rotation", CameraRotation, 0.01, -std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
		ImGui::SliderInt("FOV", &FOV, 30, 110);
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		//swap buffers and process events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	shaderProgram.Delete();
	//need to terminate glfw
	glfwTerminate();
	return 0;
}

//defenition to resize the viewport when the window is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

//simple function for processing inputs
void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}