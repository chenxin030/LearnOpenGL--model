#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnOpenGL/shader.h>
#include <learnOpenGL/Camera.h>
#include <learnOpenGL/model.h>

#include<iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(const char* path);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

glm::vec3 lightPos(1.2f,1.0f,2.0f);

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGl", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

	stbi_set_flip_vertically_on_load(true);

	Shader ourShader("model.vs.txt", "model.fs.txt");
	Shader lightCube("light_cube.vs.txt", "light_cube.fs.txt");

	float vertices[] = {
		-0.5f, -0.5f, -0.5f,//7
		 0.5f, -0.5f, -0.5f,//3
		 0.5f,  0.5f, -0.5f,//1
		 0.5f,  0.5f, -0.5f,//1
		-0.5f,  0.5f, -0.5f,//5
		-0.5f, -0.5f, -0.5f,//7

		-0.5f, -0.5f,  0.5f,//6
		 0.5f, -0.5f, -0.5f,//3
		 0.5f,  0.5f,  0.5f,//0
		 0.5f,  0.5f,  0.5f,//0
		-0.5f,  0.5f,  0.5f,//4
		-0.5f, -0.5f,  0.5f,//6

		-0.5f,  0.5f,  0.5f,//4
		-0.5f,  0.5f, -0.5f,//5
		-0.5f, -0.5f, -0.5f,//7
		-0.5f, -0.5f, -0.5f,//7
		-0.5f, -0.5f,  0.5f,//6
		-0.5f,  0.5f,  0.5f,//4

		 0.5f,  0.5f,  0.5f,//0
		 0.5f,  0.5f, -0.5f,//1
		 0.5f, -0.5f, -0.5f,//3
		 0.5f, -0.5f, -0.5f,//3
		 0.5f, -0.5f,  0.5f,//2
		 0.5f,  0.5f,  0.5f,//0

		-0.5f, -0.5f, -0.5f,//7
		 0.5f, -0.5f, -0.5f,//3
		 0.5f, -0.5f,  0.5f,//2
		 0.5f, -0.5f,  0.5f,//2
		-0.5f, -0.5f,  0.5f,//6
		-0.5f, -0.5f, -0.5f,//7

		-0.5f,  0.5f, -0.5f,//5
		 0.5f,  0.5f, -0.5f,//1
		 0.5f,  0.5f,  0.5f,//0
		 0.5f,  0.5f,  0.5f,//0
		-0.5f,  0.5f,  0.5f,//4
		-0.5f,  0.5f, -0.5f,//5
	};
	glm::vec3 lightPosition[] = {
		 glm::vec3(0.0f, 15.0f, 1.5f),
		 glm::vec3(0.0f, 0.0f, 0.0f),
	};

	Model ourModel("resource/model/nanosuit/nanosuit.obj");

	unsigned int VBO, EBO;
	unsigned int lightCubeVAO;

	glGenVertexArrays(1, &lightCubeVAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(lightCubeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ourShader.use();
		ourShader.setVec3("viewPos", camera.Position);

		ourShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		ourShader.setVec3("dirLight.ambient", 0.5f, 0.5f, 0.5f);
		ourShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
		ourShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

		ourShader.setVec3("pointLights[0].position", lightPosition[0]);
		ourShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
		ourShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
		ourShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
		ourShader.setFloat("pointLights[0].constant", 1.0f);
		ourShader.setFloat("pointLights[0].linear", 0.22f);
		ourShader.setFloat("pointLights[0].quadratic", 0.20f);

		ourShader.setVec3("pointLights[1].position", lightPosition[1]);
		ourShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
		ourShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
		ourShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
		ourShader.setFloat("pointLights[1].constant", 1.0f);
		ourShader.setFloat("pointLights[1].linear", 0.22f);
		ourShader.setFloat("pointLights[1].quadratic", 0.20f);

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		ourShader.setMat4("projection", projection);
		ourShader.setMat4("view", view);

		// render the loaded model
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
		ourShader.setMat4("model", model);
		ourModel.Draw(ourShader);

		lightCube.use();
		lightCube.setMat4("projection", projection);
		lightCube.setMat4("view", view);

		glBindVertexArray(lightCubeVAO);
		for (unsigned int i = 0; i < 2; i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, lightPosition[i]);
			model = glm::scale(model, glm::vec3(0.2f));
			lightCube.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &lightCubeVAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

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

unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}