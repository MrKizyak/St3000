#include <GL/glew.h>
#include <GL/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h> 

#include <Windows.h>
#include <thread>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SOIL/SOIL.h>

#include "shader.h"
#include "Model.h"
#include <iostream>

#include <thread>
#include <chrono>
using namespace std;

const char fragsh1[] = "shader.frag";
const char fragsh2[] = "shader2.frag";
const char vertsh2[] = "shader.vert";

bool YNping = false;

glm::vec2 resolution = glm::vec2(800.0f, 600.0f);
glm::vec2 mouse = glm::vec2(0.0f);
glm::vec3 camPos = glm::vec3(0.0f,5.0f, 0.0f); //glm::vec3 camPos = glm::vec3(10.0f,13.0f, -9.0f);
glm::vec3 camDir = glm::vec3(0.0f, 0.0f, 0.0f);  //glm::vec3 camDir = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camRight = glm::vec3(-1.0f, 0.0f, 0.0f);
glm::vec3 camUp = glm::vec3(0.0f, 0.0f, 0.0f);
glm::mat4 view;

float speed = 2.0f;
float fov = 90.0f;
float sensitivity = 0.005f; // Чуствителльность мыши

void UpdateInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

	float camSpeed = static_cast<float>(glfwGetTime()) / 1000.0f * speed;

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camSpeed *= 2.0f;

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		camSpeed /= 10.0f;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camPos += camDir * camSpeed;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camPos -= camDir * camSpeed;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camPos -= camRight * camSpeed;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camPos += camRight * camSpeed;
}



void MouseUpdate(GLFWwindow* window) {

	double xpos;
	double ypos;

	glfwGetCursorPos(window, &xpos, &ypos);

	mouse.x += (resolution.x / 2.0 - static_cast<float>(xpos)) * sensitivity;
	mouse.y += (resolution.y / 2.0 - static_cast<float>(ypos)) * sensitivity;

	glfwSetCursorPos(window, static_cast<double>(resolution.x / 2.0), static_cast<double>(resolution.y / 2.0));


	mouse.y = (mouse.y < glm::radians(-90.0f)) ? glm::radians(-90.0f) : ((mouse.y > glm::radians(90.0f)) ? glm::radians(90.0f) : mouse.y);

	camDir = glm::vec3(
		cosf(mouse.y) * sinf(mouse.x),
		sinf(mouse.y),
		cosf(mouse.y) * cosf(mouse.x)
	);

	camRight = glm::vec3(
		sinf(mouse.x - static_cast<float>(3.14f) / 2.0f),
		0.0f,
		cosf(mouse.x - static_cast<float>(3.14f) / 2.0f)
	);

	camUp = glm::cross(camRight, camDir);
}

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    fov -= static_cast<float>(yoffset);
	fov = (fov < 1.0f) ? 1.0 : ((fov > 180.0f) ? 180.0f : fov);

	printf("FOV: %5.2f\n", fov);
}

//пингуем адрес
void ipping()
{
	this_thread::sleep_for(chrono::milliseconds(30000));
	while (1) {
		//std::string ip
		this_thread::sleep_for(chrono::milliseconds(30000));
		int x = system("ping 192.168.0.140");//  > /dev/null 2>&1
		if (x == 0) {
			YNping = true;
		}
		else {
			YNping = false;
		}
	}
}



// The MAIN function, from here we start the application and run the game loop
int main()
{
	// Инициализация GLFW
	printf("Initializing GLFW...\n");
	if (glfwInit() != GLFW_TRUE) {
		printf("Error initializing GLFW\n");
		return -1;
	}

	// Настройка окна (OGL 3.2 Core Profile)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE); // Разрешить изменение размера
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE); // Включить режим отладки контекста OGL

	// Создание окна
	printf("Creating Window...\n");
	GLFWwindow* window = glfwCreateWindow(static_cast<int>(resolution.x), static_cast<int>(resolution.y), "aaaaaqqqq", nullptr, nullptr);
	if (window == nullptr) {
		MessageBoxA(NULL, "Fatal Error", "Failed to create GLFW window", MB_OK);
		glfwTerminate();
		printf("Failed to create GLFW window");
	}

	// Привязка окна как текущего
	glfwMakeContextCurrent(window);
	glfwSwapInterval(0); // Отключение интервала между сменой буферов (отключает вертикальную синхронизацию, по идее)

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetScrollCallback(window, ScrollCallback); 

	{
		int framebufferWidth;
		int framebufferHeight;
		glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
		resolution = glm::vec2(static_cast<float>(framebufferWidth), static_cast<float>(framebufferHeight));
		
		glfwSetFramebufferSizeCallback(
			window,
			[](GLFWwindow* window, int width, int height) { 
				resolution = glm::vec2(static_cast<float>(width), static_cast<float>(height));
			}
		);
	}

	// Инициализация GLEW и загрузка расширений OGL (загрузка стандарта 3.2 Core Profile)
	if (glewInit() != GLEW_OK) {
		MessageBoxA(NULL, "Fatal Error", "Failed to init GLEW", MB_OK);
		return -1;
	}

	// Загрузка модели
	Model* pModel = new Model();
	if (pModel->Load("untitled.obj") != true) {
		printf("Error load\n");
		return -1;
	}

	

	
	Shader* pShader = new Shader();
	//пинг
	
	
	if (pShader->Load("shader.frag", "shader.vert") != true) {
		printf("Error Load\n");
		return -1;
	}
	
	glm::mat4 modelMatrix;
	GLuint matrix;
	GLuint uModelMatrix;
	GLuint uTime;

	//ПИНГ В ПОТОКЕ
	std::thread th(ipping);
	th.detach();

	
	

	//Умножение матрицы вида на ортографическую матрицу (получение матрицы области вида, так-то, но ладно)
	view *= glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.5f, 100.0f) /*glm::perspective(45.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);*/;
	
	// Создание матрицы модели
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 10.0f, 0.0f));
	
	// Привязка шейдера как текущего
	pShader->Use();
	matrix = pShader->GetUniform("matrix"); // Запрос юниформы (глобальной переменной, доступной CPU) из шейдера
	glUniformMatrix4fv(matrix, 1, false, glm::value_ptr(view)); // Передача матрицы в шейдер
	
	uModelMatrix = pShader->GetUniform("model");
	glUniformMatrix4fv(uModelMatrix, 1, false, glm::value_ptr(glm::mat4(1.0f)));
	
	uTime = pShader->GetUniform("time");

	// Включение теста глубины
	glEnable(GL_DEPTH_TEST);
	// Установление цвета заливки при очистке буфера цвета
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	
	

	// Главный цикл (работать пока не будет запрос на закрытие)
	while (!glfwWindowShouldClose(window))
	{
		
		// Очистка буфера цвета и буфера глубины
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Обновляем цвет формы
		GLint vertexColorLocation = pShader->GetUniform("ourColor");
		//Проверка на подключение, и изменение цвета
		if (YNping == true) {
			glUniform4f(vertexColorLocation, 0.0f, 11.0f, 0.0f, 1.0f);
		}
		else if (YNping != true) {
			glUniform4f(vertexColorLocation, 11.0f, 0.0f, 0.0f, 1.0f);
		}
		


		// Отрисовка модели как треугольники
		pModel->Draw(GL_TRIANGLES);

		//modelMatrix = glm::rotate(glm::mat4(1.0f), static_cast<float>(glfwGetTime()) / 10.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uModelMatrix, 1, false, glm::value_ptr(modelMatrix));

		glUniform1f(uTime, static_cast<float>(glfwGetTime()));
		
		view = glm::perspective(glm::radians(fov), static_cast<float>(resolution.x / resolution.y), 0.1f, 100.0f) * glm::lookAt(
			camPos,
			camPos + camDir,
			camUp
		);

		glUniformMatrix4fv(matrix, 1, false, glm::value_ptr(view)); // Передача матрицы в шейдер
		
		// Смена буферов (отображение на экран)
		glfwSwapBuffers(window);

		// Овновление событий GLFW
		glfwPollEvents();

		UpdateInput(window);
		MouseUpdate(window);
		
	}

	delete pModel;
	delete pShader;

	glfwTerminate();

	return 0;

}